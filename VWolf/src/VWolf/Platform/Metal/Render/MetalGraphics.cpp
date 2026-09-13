//
//  MetalGraphics.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 4/19/24.
//

#include "vwpch.h"

#if defined(VWOLF_PLATFORM_MACOS) || defined(VWOLF_PLATFORM_IOS)
#include "MetalGraphics.h"

#include "HLSLMetalShader.h"

#include "VWolf/Core/Components/GameObject.h"
#include "VWolf/Core/Render/RenderItem.h"
#include "VWolf/Core/Render/RHI/Buffer.h"
#include "VWolf/Platform/Metal/Render/RHI/MetalDriver.h"
#include "VWolf/Platform/Metal/Render/RHI/MetalBufferSet.h"
#include "VWolf/Platform/Metal/Render/MetalTexture.h"
#include "VWolf/Platform/Metal/Core/SmartPointers.h"

#include "VWolf/Core/Debug/ShapeHelper.h"

namespace VWolf {
    static RHI::RenderPrimitive GetTopologyEx(Topology topology) {
        switch (topology) {
            case Topology::Triangles:
                return RHI::RenderPrimitive::Triangle;
            case Topology::Lines:
                return RHI::RenderPrimitive::Line;
            case Topology::LinesStrip:
                return RHI::RenderPrimitive::LineStrip;
            case Topology::Points:
                return RHI::RenderPrimitive::Point;
            default: return RHI::RenderPrimitive::Triangle;
        }
    }

    void MetalGraphics::Initialize() {
        InternalGraphics::Initialize();
        emptyShadowMap = std::dynamic_pointer_cast<MetalTexture2D>(CreateRef<Texture2D>(UUID::NewUUID(), TextureDefault::White, 1024, 1024, TextureOptions())->GetInnerTexture());
        shadowMap = CreateRef<MetalRenderTexture>(1024, 1024, true);
        
        MTL::CaptureManager* captureManager = MTL::CaptureManager::sharedCaptureManager();
        captureScope = CreateMetalRef<MTL::CaptureScope>(
            captureManager->newCaptureScope(MetalDriver::GetCurrent()->GetCommandQueue()->GetCommandQueue().get()));
        captureScope->setLabel(NSSTRINGFROM("VWolf Frame"));
        captureManager->setDefaultCaptureScope(captureScope.get());

    }

    void MetalGraphics::ClearColorImpl(Color color) {
        MetalDriver::GetCurrent()->GetMainRenderPass()->GetRenderPassDescriptor()->colorAttachments()->object(0)->setClearColor(MTL::ClearColor::Make(color.GetR(), color.GetG(), color.GetB(), color.GetA()));
        if (m_p_renderTexture) {
            auto metalRenderTexture = (MetalRenderTexture*)m_p_renderTexture.get();
            metalRenderTexture->GetRenderPass()->GetRenderPassDescriptor()->colorAttachments()->object(0)->setClearColor(MTL::ClearColor::Make(color.GetR(), color.GetG(), color.GetB(), color.GetA()));
        }
    }

    void MetalGraphics::EndProcessingFrame() {
        if (m_p_renderTexture) {
            ((MetalRenderTexture*)m_p_renderTexture.get())->Commit();
        }
        
        commandList->GetCommandBuffer()->presentDrawable(MetalDriver::GetCurrent()->GetSurfaceColorTexture()->GetCurrentDrawable().get());
        commandList->Commit();
        
        captureScope->endScope();

        pool->release();
        pool = nullptr;
        
        InternalGraphics::EndProcessingFrame();
    }

    void MetalGraphics::BeginProcessingFrame() {
        pool = NS::AutoreleasePool::alloc()->init();
        captureScope->beginScope();

        MetalDriver::GetCurrent()->GetMainRenderPass()->Prepare();
        commandList = DynamicAs<RHI::MetalRenderCommandList>(MetalDriver::GetCurrent()->GetCommandQueue()
                                                        ->CreateRenderCommandList(MetalDriver::GetCurrent()->GetMainRenderPass()->weak_from_this()));
        
        if (m_p_renderTexture) {
            ((MetalRenderTexture*)m_p_renderTexture.get())->Prepare();
        }
        shadowMap->Prepare();
        
        ClearColorImpl(GraphicsContext::GetBackgroundColor());
        
        commandContext = GraphicsContext::ProcessContext();

        DrawShadowMap();
        DrawQueue();
        DrawPostProcess();
        
    }

    void MetalGraphics::DrawShadowMap() {
        Ref<RHI::MetalRenderCommandList> cmdList = shadowMap->GetCommandList();
        Ref<HLSLMetalShader> shader;
        
        shader = DynamicAs<HLSLMetalShader>(commandContext->shadowShader->GetInternalShader());
        cmdList->SetRenderState(shader->GetState());
        
        for(auto [key, value]: commandContext->lightSpaceMatrix) {
            UUID lightId = std::get<1>(key);
            
            Ref<RHI::Buffer> lightSpaceMatrix;
            
            std::string lightSpaceMatrixId = "shadow-camera-" + lightId.ToString();
            
            if (MetalDriver::GetCurrent()->GetObjectRegistry()->HasKey(lightSpaceMatrixId)) {
                lightSpaceMatrix = MetalDriver::GetCurrent()->GetObjectRegistry()->Get<RHI::MetalBuffer>(lightSpaceMatrixId);
            } else {
                RHI::BufferSettings settings = RHI::BufferSettings::ForConstantBuffer(sizeof(Matrix4x4));
                // TODO: This is what I'm saying it should added as part of the context
                lightSpaceMatrix = MetalDriver::GetCurrent()->GetRenderContext()->CreateBuffer(settings);
                lightSpaceMatrix->SetName(lightSpaceMatrixId);
                MetalDriver::GetCurrent()->GetObjectRegistry()->AddObject(lightSpaceMatrix);
            }
            
            RHI::SubResource resource(&value, {});
            lightSpaceMatrix->SetData(resource);
            
            uint32_t objectIndex = 0;
            for (const GraphicsCommandEntity& entity: commandContext->entities) {
                if (!entity.castShadows) continue;
                
                if (commandContext->EntityContainsLight(entity, lightId)) {
                    Ref<RHI::MetalBuffer> indexBuffer;
                    Ref<RHI::MetalBufferSet> vertexBufferSet;
                    
                    entity.mesh->BuildVertexBuffer(shader->GetAttributes());
                    
                    // Mesh
                    if (MetalDriver::GetCurrent()->GetObjectRegistry()->HasKey("vertexSet-" + entity.mesh->GetID().ToString())) {
                        vertexBufferSet = MetalDriver::GetCurrent()->GetObjectRegistry()->Get<RHI::MetalBufferSet>("vertexSet-" + entity.mesh->GetID().ToString());
                    } else {
                        std::vector<Ref<RHI::Buffer>> buffers;
                        std::vector<float> vertices = entity.mesh->GetNativeVector();
                        
                        if (vertices.size() != 0) {
                            RHI::BufferSettings vertexSettings = RHI::BufferSettings::ForVertextBuffer((uint32_t)vertices.size() * sizeof(float));
                            Ref<RHI::Buffer> vertexBuffer = MetalDriver::GetCurrent()->GetRenderContext()->CreateBuffer(vertexSettings);
                            RHI::SubResource resourceVertices(vertices.data(), {});
                            vertexBuffer->SetData(resourceVertices);
                            buffers.push_back(vertexBuffer);
                        }
                        vertexBufferSet = DynamicAs<RHI::MetalBufferSet>(MetalDriver::GetCurrent()->GetRenderContext()->CreateBufferSet(buffers));
                        vertexBufferSet->SetName("vertexSet-" + entity.mesh->GetID().ToString());
                        MetalDriver::GetCurrent()->GetObjectRegistry()->AddObject(vertexBufferSet);
                    }
                    
                    if (MetalDriver::GetCurrent()->GetObjectRegistry()->HasKey("index-" + entity.mesh->GetID().ToString())) {
                        indexBuffer = MetalDriver::GetCurrent()->GetObjectRegistry()->Get<RHI::MetalBuffer>("index-" + entity.mesh->GetID().ToString());
                    } else {
                        std::vector<uint32_t> triangles = entity.mesh->GetTriangles();
                        RHI::BufferSettings indexSettings = RHI::BufferSettings::ForIndexBuffer((uint32_t)triangles.size(), RHI::PixelFormat::R32Uint);
                        indexBuffer = DynamicAs<RHI::MetalBuffer>(MetalDriver::GetCurrent()->GetRenderContext()->CreateBuffer(indexSettings));
                        RHI::SubResource resourceIndices(triangles.data(), {});
                        indexBuffer->SetData(resourceIndices);
                        indexBuffer->SetName("index-" + entity.mesh->GetID().ToString());
                        MetalDriver::GetCurrent()->GetObjectRegistry()->AddObject(indexBuffer);
                    }
                    
                    cmdList->SetVertexBuffers(vertexBufferSet);
                    cmdList->SetIndexBuffer(indexBuffer);
                    
                    std::map<std::string, Ref<RHI::BindableProgramArgument>> arguments = shader->GetProgram()->GetSettings().GetProgramArguments();
                    std::unordered_map<Ref<RHI::BindableProgramArgument>, RHI::ProgramArgumentBindingValue, RHI::BindableProgramArgument::Hash> parameters;
                    
                    for(const auto& [key, value]: arguments) {
                        
                        if (value->GetResourceType() == RHI::ResourceType::Buffer) {
                            Ref<RHI::ConstantBufferProgramArgument> constantArgument = DynamicAs<RHI::ConstantBufferProgramArgument>(value);
                            
                            if ((std::string("cbPer") + Shader::CameraBufferName) == key) {
                                parameters[value] = lightSpaceMatrix->GetResourceView();
                            } else  if ((std::string("cbPer") + Shader::ObjectBufferName) == key) {
                                const Matrix4x4& objectMatrix = entity.transform;
                                Ref<RHI::Buffer> buffer;
                                
                                std::string objectKey = std::string("shadow-cbPer") + Shader::ObjectBufferName + "-" + std::to_string(objectIndex);
                                
                                if (MetalDriver::GetCurrent()->GetObjectRegistry()->HasKey(objectKey)) {
                                    buffer = MetalDriver::GetCurrent()->GetObjectRegistry()->Get<RHI::MetalBuffer>(objectKey);
                                } else {
                                    RHI::BufferSettings settings = RHI::BufferSettings::ForConstantBuffer((uint32_t)constantArgument->GetSize());
                                    // TODO: This is what I'm saying it should added as part of the context
                                    buffer = MetalDriver::GetCurrent()->GetRenderContext()->CreateBuffer(settings);
                                    buffer->SetName(objectKey);
                                    MetalDriver::GetCurrent()->GetObjectRegistry()->AddObject(indexBuffer);
                                }
                                
                                RHI::SubResource resource(&objectMatrix, {});
                                buffer->SetData(resource);
                                parameters[value] = buffer->GetResourceView();
                            }
                        }
                    }
                    
                    RHI::ProgramArgumentBindings bindings(shader->GetProgram(), parameters);
                    //
                    cmdList->SetProgramBindings(bindings);
                    
                    cmdList->DrawIndexed(
                                         GetTopologyEx(entity.mesh->GetSubMesh(entity.subMeshIndex).GetTopology()),
                                         indexBuffer->GetSettings().GetCount()
                                         );
                    objectIndex++;
                }
            }
        }
        shadowMap->Commit();
    }

    void MetalGraphics::DrawQueue() {
        
        Ref<RHI::MetalRenderCommandList> cmdList = m_p_renderTexture ? ((MetalRenderTexture*)m_p_renderTexture.get())->GetCommandList() : commandList;
        uint32_t commandIndex = 0;
        
        for(Ref<CameraComponent> cameraComponent: commandContext->cameras) {
            const CameraPass& cameraPass = commandContext->gpuCamera[cameraComponent->GetID()];
            const CameraPass& skyBoxCameraPass = commandContext->gpuSkyboxCamera[cameraComponent->GetID()];
            
            Ref<RHI::Buffer> cameraBuffer;
            std::string cameraKey = "queue-camera-" + cameraComponent->GetID().ToString();
            
            if (MetalDriver::GetCurrent()->GetObjectRegistry()->HasKey(cameraKey)) {
                cameraBuffer = MetalDriver::GetCurrent()->GetObjectRegistry()->Get<RHI::MetalBuffer>(cameraKey);
            } else {
                RHI::BufferSettings settings = RHI::BufferSettings::ForConstantBuffer(sizeof(CameraPass));
                // TODO: This is what I'm saying it should added as part of the context
                cameraBuffer = MetalDriver::GetCurrent()->GetRenderContext()->CreateBuffer(settings);
                cameraBuffer->SetName(cameraKey);
                MetalDriver::GetCurrent()->GetObjectRegistry()->AddObject(cameraBuffer);
            }
            RHI::SubResource resource(&cameraPass, {});
            cameraBuffer->SetData(resource);
            
            Ref<RHI::Buffer> skyboxCameraBuffer;
            std::string skyboxCameraKey = "queue-skybox-camera-" + cameraComponent->GetID().ToString();
            
            if (MetalDriver::GetCurrent()->GetObjectRegistry()->HasKey(skyboxCameraKey)) {
                skyboxCameraBuffer = MetalDriver::GetCurrent()->GetObjectRegistry()->Get<RHI::MetalBuffer>(skyboxCameraKey);
            } else {
                RHI::BufferSettings settings = RHI::BufferSettings::ForConstantBuffer(sizeof(CameraPass));
                // TODO: This is what I'm saying it should added as part of the context
                skyboxCameraBuffer = MetalDriver::GetCurrent()->GetRenderContext()->CreateBuffer(settings);
                skyboxCameraBuffer->SetName(skyboxCameraKey);
                MetalDriver::GetCurrent()->GetObjectRegistry()->AddObject(skyboxCameraBuffer);
            }
            RHI::SubResource skyboxResource(&skyBoxCameraPass, {});
            skyboxCameraBuffer->SetData(skyboxResource);
            
            std::vector<Ref<Shader>> sorted_elements(commandContext->shaders.begin(), commandContext->shaders.end());
            
            std::sort(sorted_elements.begin(), sorted_elements.end(), [](Ref<Shader> shaderA, Ref<Shader> shaderB) {
                return shaderA->GetSubShader().GetPriority() < shaderB->GetSubShader().GetPriority();
            });
            
            for(const Ref<Shader>& shaderAsset: sorted_elements) {
                Ref<HLSLMetalShader> metalShader = DynamicAs<HLSLMetalShader>(shaderAsset->GetInternalShader());
                cmdList->SetRenderState(metalShader->GetState());
                
                bool isSkybox = shaderAsset->GetSubShader().GetPriority() < 200;
                
                std::set<UUID> materialsIds = commandContext->shaderMaterials[shaderAsset->GetID()];
                
                for (UUID materialId: materialsIds) {
                    
                    Ref<Material> material = *std::find_if(commandContext->materials.begin(), commandContext->materials.end(), [materialId](Ref<Material> material) {
                        return material->GetID() == materialId;
                    });
                    
                    Ref<RHI::Buffer> materialBuffer;
                    
                    if (material->GetSize() > 0) {
                        const UniqueVoidPtr& material1 = commandContext->materialData[materialId];
                        std::string materialKey = "queue-material-" + material->GetName();
                        
                        if (MetalDriver::GetCurrent()->GetObjectRegistry()->HasKey(materialKey)) {
                            materialBuffer = MetalDriver::GetCurrent()->GetObjectRegistry()->Get<RHI::MetalBuffer>(materialKey);
                        } else {
                            RHI::BufferSettings settings = RHI::BufferSettings::ForConstantBuffer((uint32_t)material->GetSize());
                            // TODO: This is what I'm saying it should added as part of the context
                            materialBuffer = MetalDriver::GetCurrent()->GetRenderContext()->CreateBuffer(settings);
                            materialBuffer->SetName(materialKey);
                            MetalDriver::GetCurrent()->GetObjectRegistry()->AddObject(materialBuffer);
                        }
                        RHI::SubResource resource(material1.get(), {});
                        materialBuffer->SetData(resource);
                    }
                    
                    auto objectRange = commandContext->materialObjects.equal_range(materialId);
                    for (auto objectIt = objectRange.first; objectIt != objectRange.second; ++objectIt) {
                        UUID objectId = objectIt->second;
                        
                        std::vector<Light> lights(Light::LightsMax);
                        std::vector<Matrix4x4> lightSpaces(Light::LightsMax);
                        
                        auto lightsRange = commandContext->objectLights.equal_range(objectId);
                        
                        int lightIndex = -1;
                        for (auto lightIt = lightsRange.first; lightIt != lightsRange.second; ++lightIt) {
                            
                            lightIndex++;
                            
                            UUID lightId = lightIt->second;
                            auto it = std::find_if(commandContext->lights.begin(), commandContext->lights.end(), [lightId](const Ref<LightComponent> lightComponent) {
                                return lightComponent->GetID() == lightId;
                            });
                            
                            if (it == commandContext->lights.end()) continue;
                            
                            std::tuple<UUID, UUID> lightSpaceKey = std::make_tuple(cameraComponent->GetID(), lightId);
                            lightSpaces[lightIndex] = commandContext->lightSpaceMatrix[lightSpaceKey];
                            
                            Ref<LightComponent> lightComponent = *it;
                            
                            lights[lightIndex] = lightComponent->GetLight();
                        }
                        
                        GraphicsCommandEntity entity = *std::find_if(commandContext->entities.begin(), commandContext->entities.end(), [objectId](const GraphicsCommandEntity& entity) {
                            return entity.id == objectId;
                        });
                        entity.mesh->BuildVertexBuffer(metalShader->GetAttributes());
                        
                        Ref<RHI::MetalBuffer> indexBuffer;
                        Ref<RHI::MetalBufferSet> vertexBufferSet;
                        
                        if (MetalDriver::GetCurrent()->GetObjectRegistry()->HasKey("vertexSet-" + entity.mesh->GetID().ToString())) {
                            vertexBufferSet = MetalDriver::GetCurrent()->GetObjectRegistry()->Get<RHI::MetalBufferSet>("vertexSet-" + entity.mesh->GetID().ToString());
                        } else {
                            std::vector<Ref<RHI::Buffer>> buffers;
                            std::vector<float> vertices = entity.mesh->GetNativeVector();
                            
                            if (vertices.size() != 0) {
                                RHI::BufferSettings vertexSettings = RHI::BufferSettings::ForVertextBuffer((uint32_t)vertices.size() * sizeof(float));
                                Ref<RHI::Buffer> vertexBuffer = MetalDriver::GetCurrent()->GetRenderContext()->CreateBuffer(vertexSettings);
                                RHI::SubResource resourceVertices(vertices.data(), {});
                                vertexBuffer->SetData(resourceVertices);
                                buffers.push_back(vertexBuffer);
                            }
                            vertexBufferSet = DynamicAs<RHI::MetalBufferSet>(MetalDriver::GetCurrent()->GetRenderContext()->CreateBufferSet(buffers));
                            vertexBufferSet->SetName("vertexSet-" + entity.mesh->GetID().ToString());
                            MetalDriver::GetCurrent()->GetObjectRegistry()->AddObject(vertexBufferSet);
                        }
                        
                        if (MetalDriver::GetCurrent()->GetObjectRegistry()->HasKey("index-" + entity.mesh->GetID().ToString())) {
                            indexBuffer = MetalDriver::GetCurrent()->GetObjectRegistry()->Get<RHI::MetalBuffer>("index-" + entity.mesh->GetID().ToString());
                        } else {
                            std::vector<uint32_t> triangles = entity.mesh->GetTriangles();
                            RHI::BufferSettings indexSettings = RHI::BufferSettings::ForIndexBuffer((uint32_t)triangles.size(), RHI::PixelFormat::R32Uint);
                            indexBuffer = DynamicAs<RHI::MetalBuffer>(MetalDriver::GetCurrent()->GetRenderContext()->CreateBuffer(indexSettings));
                            RHI::SubResource resourceIndices(triangles.data(), {});
                            indexBuffer->SetData(resourceIndices);
                            indexBuffer->SetName("index-" + entity.mesh->GetID().ToString());
                            MetalDriver::GetCurrent()->GetObjectRegistry()->AddObject(indexBuffer);
                        }
                        
                        if (vertexBufferSet->GetBuffers().size() > 0)
                            cmdList->SetVertexBuffers(vertexBufferSet);
                        cmdList->SetIndexBuffer(indexBuffer);
                        
                        std::map<std::string, Ref<RHI::BindableProgramArgument>> arguments = metalShader->GetProgram()->GetSettings().GetProgramArguments();
                        std::unordered_map<Ref<RHI::BindableProgramArgument>, RHI::ProgramArgumentBindingValue, RHI::BindableProgramArgument::Hash> parameters;
                        
                        for(const auto& [key, value]: arguments) {
                            
                            if (value->GetResourceType() == RHI::ResourceType::Buffer) {
                                Ref<RHI::ConstantBufferProgramArgument> constantArgument = DynamicAs<RHI::ConstantBufferProgramArgument>(value);
                                
                                if ((std::string("cbPer") + Shader::CameraBufferName) == key) {
                                    parameters[value] = isSkybox ? skyboxCameraBuffer->GetResourceView() : cameraBuffer->GetResourceView();
                                }
                                if ((std::string("cbPer") + materialName) == key && materialBuffer != nullptr) {
                                    parameters[value] = materialBuffer->GetResourceView();
                                }
                                if ((std::string("cbPer") + Shader::ObjectBufferName) == key) {
                                    Ref<RHI::Buffer> objectBuffer;
                                    std::string objectKey = "queue-object-" + std::to_string(commandIndex);
                                    
                                    if (MetalDriver::GetCurrent()->GetObjectRegistry()->HasKey(objectKey)) {
                                        objectBuffer = MetalDriver::GetCurrent()->GetObjectRegistry()->Get<RHI::MetalBuffer>(objectKey);
                                    } else {
                                        RHI::BufferSettings settings = RHI::BufferSettings::ForConstantBuffer((uint32_t)constantArgument->GetSize());
                                        // TODO: This is what I'm saying it should added as part of the context
                                        objectBuffer = MetalDriver::GetCurrent()->GetRenderContext()->CreateBuffer(settings);
                                        objectBuffer->SetName(objectKey);
                                        MetalDriver::GetCurrent()->GetObjectRegistry()->AddObject(objectBuffer);
                                    }
                                    Matrix4x4 object = entity.transform;
                                    RHI::SubResource resource(&object, {});
                                    objectBuffer->SetData(resource);
                                    parameters[value] = objectBuffer->GetResourceView();
                                }
                                if ((std::string("cbPer") + Light::LightName) == key) {
                                    Ref<RHI::Buffer> lightsBuffer;
                                    std::string ligthsKey = "queue-lights";
                                    
                                    if (MetalDriver::GetCurrent()->GetObjectRegistry()->HasKey(ligthsKey)) {
                                        lightsBuffer = MetalDriver::GetCurrent()->GetObjectRegistry()->Get<RHI::MetalBuffer>(ligthsKey);
                                    } else {
                                        RHI::BufferSettings settings = RHI::BufferSettings::ForConstantBuffer((uint32_t)constantArgument->GetSize());
                                        // TODO: This is what I'm saying it should added as part of the context
                                        lightsBuffer = MetalDriver::GetCurrent()->GetRenderContext()->CreateBuffer(settings);
                                        lightsBuffer->SetName(ligthsKey);
                                        MetalDriver::GetCurrent()->GetObjectRegistry()->AddObject(lightsBuffer);
                                    }
                                    RHI::SubResource resource(lights.data(), {});
                                    lightsBuffer->SetData(resource);
                                    parameters[value] = lightsBuffer->GetResourceView();
                                }
                                if ((std::string("cbPer") + Light::LightSpaceName) == key) {
                                    Ref<RHI::Buffer> lightsSpacesBuffer;
                                    std::string lightsSpacesKey = "queue-lights-spaces";
                                    
                                    if (MetalDriver::GetCurrent()->GetObjectRegistry()->HasKey(lightsSpacesKey)) {
                                        lightsSpacesBuffer = MetalDriver::GetCurrent()->GetObjectRegistry()->Get<RHI::MetalBuffer>(lightsSpacesKey);
                                    } else {
                                        RHI::BufferSettings settings = RHI::BufferSettings::ForConstantBuffer((uint32_t)constantArgument->GetSize());
                                        // TODO: This is what I'm saying it should added as part of the context
                                        lightsSpacesBuffer = MetalDriver::GetCurrent()->GetRenderContext()->CreateBuffer(settings);
                                        lightsSpacesBuffer->SetName(lightsSpacesKey);
                                        MetalDriver::GetCurrent()->GetObjectRegistry()->AddObject(lightsSpacesBuffer);
                                    }
                                    
                                    RHI::SubResource resource(lightSpaces.data(), {});
                                    lightsSpacesBuffer->SetData(resource);
                                    parameters[value] = lightsSpacesBuffer->GetResourceView();
                                }
                                
                            } else if (value->GetResourceType() == RHI::ResourceType::Texture) {
                                Ref<RHI::TextureProgramArgument> texArgument = DynamicAs<RHI::TextureProgramArgument>(value);
                                Ref<RHI::Texture> rhiTexture;
                                if (value->GetName() == "Shadow") {
                                    rhiTexture = shadowMap->GetRHIDepthTexture();
                                } else {
                                    if (texArgument->GetTextureDimensionType() == RHI::TextureDimensionType::Cube) {
                                        Ref<Cubemap> assetCubemap = DynamicAs<Cubemap>(material->GetTexture(value->GetName()));
                                        Ref<MetalCubemap> metalCubemap = DynamicAs<MetalCubemap>(assetCubemap->GetInnerTexture());
                                        rhiTexture = metalCubemap->GetRHITexture();
                                    }
                                    if (texArgument->GetTextureDimensionType() == RHI::TextureDimensionType::Tex2D) {
                                        Ref<Texture2D> assetTexture = DynamicAs<Texture2D>(material->GetTexture(value->GetName()));
                                        Ref<MetalTexture2D> metalTexture2D = DynamicAs<MetalTexture2D>(assetTexture->GetInnerTexture());
                                        rhiTexture = metalTexture2D->GetRHITexture();
                                    }
                                }
                                parameters[value] = rhiTexture->GetTextureView({});
                            }
                        }
                        
                        /*
                         Now that we pass the state to the command list, the next part would be
                         1. Passing the program argument bindings // DONE
                         2. Passing the vertex buffer // DONE
                         3. Passsing the index buffer // DONE
                         4. Drawing // DONE
                         
                         The way the shader is buildind the descriptor tables should not live inside the asset class.
                         It should live somewhere else. The question is where.
                         
                         Edit Sept 9th 26: I understood how the descriptor tables work now. I need to improve the code on how to
                         keep them alive.
                         */
                        
                        RHI::ProgramArgumentBindings bindings(
                                                              metalShader->GetProgram(), parameters);
                        //
                        cmdList->SetProgramBindings(bindings);
                        
                        cmdList->DrawIndexed(
                                             GetTopologyEx(entity.mesh->GetSubMesh(entity.subMeshIndex).GetTopology()),
                                             indexBuffer->GetSettings().GetCount()
                                             );
                        commandIndex++;
                    }
                }
            }
        }
    }

    void MetalGraphics::DrawPostProcess() {
        
    }
}

#endif
