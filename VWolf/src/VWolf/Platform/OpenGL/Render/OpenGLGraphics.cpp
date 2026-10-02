//
//  OpenGLRenderer.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 10/11/22.
//

#include "vwpch.h"
#include "OpenGLGraphics.h"
#include "HLSLOpenGLShader.h"

#include "VWolf/Core/Components/GameObject.h"
#include "VWolf/Core/Render/RenderItem.h"
#include "VWolf/Core/Render/GraphicsContext.h"

#include "VWolf/Platform/OpenGL/Core/GLCore.h"

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

    static GLenum GetTopology(Topology topology) {
        switch (topology) {
            case Topology::Triangles:
                return GL_TRIANGLES;
            case Topology::Lines:
                return GL_LINES;
            case Topology::LinesStrip:
                return GL_LINE_STRIP;
            case Topology::Points:
                return GL_POINTS;
            default: return -1;
        }
    }

    void OpenGLGraphics::Initialize() {
        InternalGraphics::Initialize();
        shadowMap = CreateRef<OpenGLRenderTexture>(1024, 1024, true, TextureOptions());
        emptyShadowMap = std::dynamic_pointer_cast<OpenGLTexture2D>(CreateRef<Texture2D>(UUID::NewUUID(), TextureDefault::White, 1024, 1024, TextureOptions())->GetInnerTexture());
    }

    void OpenGLGraphics::BindToRenderTexture() {
        if (m_p_renderTexture != nullptr && useRenderTexture) {
            ((OpenGLRenderTexture*)m_p_renderTexture.get())->Bind();
        }
    }

    void OpenGLGraphics::UnbindToRenderTexture(){
        if (m_p_renderTexture != nullptr && useRenderTexture) {
            ((OpenGLRenderTexture*)m_p_renderTexture.get())->Unbind();
        }
    }

    void OpenGLGraphics::ClearColorImpl(Color color) {
        BindToRenderTexture();
        GLThrowIfFailed(glClearColor(color.GetR(), color.GetG(), color.GetB(), color.GetA()));
        UnbindToRenderTexture();
    }

    void OpenGLGraphics::ClearImpl() {
        BindToRenderTexture();
        GLThrowIfFailed(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
        UnbindToRenderTexture();
    }

    void OpenGLGraphics::EndProcessingFrame()
    {
        InternalGraphics::EndProcessingFrame();
    }

    void OpenGLGraphics::BeginProcessingFrame() {
        useRenderTexture = true;
        
        ClearColorImpl(GraphicsContext::GetBackgroundColor());
        // TODO: What to do with the clear statement
        ClearImpl();
        
        if (m_p_renderTexture) {
            ((OpenGLRenderTexture*)m_p_renderTexture.get())->Prepare();
        }
        shadowMap->Prepare();
        commandContext = GraphicsContext::ProcessContext();
        
        DrawShadowMap();
        DrawQueue();
        DrawPostProcess();
        useRenderTexture = false;
    }

    void OpenGLGraphics::DrawShadowMap() {
        
        Ref<RHI::OpenGLRenderCommandList> cmdList = shadowMap->GetCommandList();
        // TODO: What to do with the clear statement
        GLThrowIfFailed(glClear(GL_DEPTH_BUFFER_BIT));
        Ref<HLSLOpenGLShader> shader;
        
        shader = DynamicAs<HLSLOpenGLShader>(commandContext->shadowShader->GetInternalShader());
        cmdList->SetRenderState(shader->GetState());
        
        for(auto [key, value]: commandContext->lightSpaceMatrix) {
            UUID lightId = std::get<1>(key);
            
            Ref<RHI::Buffer> lightSpaceMatrix;
            
            std::string lightSpaceMatrixId = "shadow-camera-" + lightId.ToString();
            
            if (OpenGLDriver::GetCurrentDriver()->GetObjectRegistry()->HasKey(lightSpaceMatrixId)) {
                lightSpaceMatrix = OpenGLDriver::GetCurrentDriver()->GetObjectRegistry()->Get<RHI::OpenGLBuffer>(lightSpaceMatrixId);
            } else {
                RHI::BufferSettings settings = RHI::BufferSettings::ForConstantBuffer(sizeof(Matrix4x4));
                // TODO: This is what I'm saying it should added as part of the context
                lightSpaceMatrix = OpenGLDriver::GetCurrentDriver()->GetRenderContext()->CreateBuffer(settings);
                lightSpaceMatrix->SetName(lightSpaceMatrixId);
                OpenGLDriver::GetCurrentDriver()->GetObjectRegistry()->AddObject(lightSpaceMatrix);
            }
            
            RHI::SubResource resource(&value, {});
            lightSpaceMatrix->SetData(resource);
            
            uint32_t objectIndex = 0;
            for (const GraphicsCommandEntity& entity: commandContext->entities) {
                if (!entity.castShadows) continue;
                
                if (commandContext->EntityContainsLight(entity, lightId)) {
                    Ref<RHI::OpenGLBuffer> indexBuffer;
                    Ref<RHI::OpenGLBufferSet> vertexBufferSet;
                    
                    entity.mesh->BuildVertexBuffer(shader->GetAttributes());
                    
                    // Mesh
                    if (OpenGLDriver::GetCurrentDriver()->GetObjectRegistry()->HasKey("vertexSet-" + entity.mesh->GetID().ToString())) {
                        vertexBufferSet = OpenGLDriver::GetCurrentDriver()->GetObjectRegistry()->Get<RHI::OpenGLBufferSet>("vertexSet-" + entity.mesh->GetID().ToString());
                    } else {
                        std::vector<Ref<RHI::Buffer>> buffers;
                        std::vector<float> vertices = entity.mesh->GetNativeVector();
                        
                        if (vertices.size() != 0) {
                            RHI::BufferSettings vertexSettings = RHI::BufferSettings::ForVertextBuffer((uint32_t)vertices.size() * sizeof(float));
                            Ref<RHI::Buffer> vertexBuffer = OpenGLDriver::GetCurrentDriver()->GetRenderContext()->CreateBuffer(vertexSettings);
                            RHI::SubResource resourceVertices(vertices.data(), {});
                            vertexBuffer->SetData(resourceVertices);
                            buffers.push_back(vertexBuffer);
                        }
                        vertexBufferSet = DynamicAs<RHI::OpenGLBufferSet>(OpenGLDriver::GetCurrentDriver()->GetRenderContext()->CreateBufferSet(buffers));
                        vertexBufferSet->SetName("vertexSet-" + entity.mesh->GetID().ToString());
                        OpenGLDriver::GetCurrentDriver()->GetObjectRegistry()->AddObject(vertexBufferSet);
                    }
                    
                    if (OpenGLDriver::GetCurrentDriver()->GetObjectRegistry()->HasKey("index-" + entity.mesh->GetID().ToString())) {
                        indexBuffer = OpenGLDriver::GetCurrentDriver()->GetObjectRegistry()->Get<RHI::OpenGLBuffer>("index-" + entity.mesh->GetID().ToString());
                    } else {
                        std::vector<uint32_t> triangles = entity.mesh->GetTriangles();
                        RHI::BufferSettings indexSettings = RHI::BufferSettings::ForIndexBuffer((uint32_t)triangles.size(), RHI::PixelFormat::R32Uint);
                        indexBuffer = DynamicAs<RHI::OpenGLBuffer>(OpenGLDriver::GetCurrentDriver()->GetRenderContext()->CreateBuffer(indexSettings));
                        RHI::SubResource resourceIndices(triangles.data(), {});
                        indexBuffer->SetData(resourceIndices);
                        indexBuffer->SetName("index-" + entity.mesh->GetID().ToString());
                        OpenGLDriver::GetCurrentDriver()->GetObjectRegistry()->AddObject(indexBuffer);
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
                                
                                if (OpenGLDriver::GetCurrentDriver()->GetObjectRegistry()->HasKey(objectKey)) {
                                    buffer = OpenGLDriver::GetCurrentDriver()->GetObjectRegistry()->Get<RHI::OpenGLBuffer>(objectKey);
                                } else {
                                    RHI::BufferSettings settings = RHI::BufferSettings::ForConstantBuffer((uint32_t)constantArgument->GetSize());
                                    // TODO: This is what I'm saying it should added as part of the context
                                    buffer = OpenGLDriver::GetCurrentDriver()->GetRenderContext()->CreateBuffer(settings);
                                    buffer->SetName(objectKey);
                                    OpenGLDriver::GetCurrentDriver()->GetObjectRegistry()->AddObject(indexBuffer);
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
        
//        shadowMap->Bind();
        
//        HLSLOpenGLShader* shader = (HLSLOpenGLShader*) Shader::GetShader("Shadow")->GetInternalShader().get();
//        for (const Matrix4x4& viewProjection: GraphicsContext::GetLightsSpaces()) {
//
//            for(Ref<GraphicsCommand> command: GraphicsContext::GetList().GetCommands()) {
//                Ref<DrawMeshCommand> drawMeshCommand = std::dynamic_pointer_cast<DrawMeshCommand>(command);
//                if (!drawMeshCommand->GetCastShadows()) continue;
//                
//                Ref<Mesh> mesh1 = drawMeshCommand->GetMesh();
//                
//                if (mesh1 == nullptr || mesh1->GetVertices().size() == 1) return;; // It's a light
//
//                mesh1->BuildVertexBuffer(shader->GetAttributes());
//                
//                auto data = mesh1->GetNativeVector();
//                if (data.size() == 1) continue;; // It's a light
//                auto indices = mesh1->GetTriangles();
//                
//                Ref<OpenGLVertexBuffer> vertices = CreateRef<OpenGLVertexBuffer>(data.data(), data.size() * sizeof(float));
//                Ref<OpenGLIndexBuffer> index = CreateRef<OpenGLIndexBuffer>(indices.data(), indices.size());
//                Ref<OpenGLVertexArray> group = CreateRef<OpenGLVertexArray>(vertices, shader->GetAttributes());
//
//                
//                Matrix4x4 transform = drawMeshCommand->GetTransform();
//                
//                shader->Bind();
//                shader->SetData(&viewProjection, Shader::CameraBufferName, sizeof(Matrix4x4), 0);
//                shader->SetData(&transform, Shader::ObjectBufferName, sizeof(Matrix4x4), 0);
//
//                group->Bind();
//                vertices->Bind();
//                index->Bind();
//                
//                uint32_t count = index->GetCount();
//                GLThrowIfFailed(glDrawElements(GetTopology(mesh1->GetSubMesh(0).GetTopology()), count, GL_UNSIGNED_INT, nullptr));
//
//                vertices->Unbind();
//                index->Unbind();
//                group->Unbind();
//                shader->Unbind();
//            }
//        }
//        shadowMap->Unbind();
    }

    void OpenGLGraphics::DrawQueue() {
        std::cout << "Drawing queue in frame: " << Time::GetTotalFrames() << std::endl;
        
        
        Ref<RHI::OpenGLRenderCommandList> cmdList =((OpenGLRenderTexture*)m_p_renderTexture.get())->GetCommandList();
        uint32_t commandIndex = 0;
        
        for(Ref<CameraComponent> cameraComponent: commandContext->cameras) {
            const CameraPass& cameraPass = commandContext->gpuCamera[cameraComponent->GetID()];
            const CameraPass& skyBoxCameraPass = commandContext->gpuSkyboxCamera[cameraComponent->GetID()];
            
            Ref<RHI::Buffer> cameraBuffer;
            std::string cameraKey = "queue-camera-" + cameraComponent->GetID().ToString();
            
            if (OpenGLDriver::GetCurrentDriver()->GetObjectRegistry()->HasKey(cameraKey)) {
                cameraBuffer = OpenGLDriver::GetCurrentDriver()->GetObjectRegistry()->Get<RHI::OpenGLBuffer>(cameraKey);
            } else {
                RHI::BufferSettings settings = RHI::BufferSettings::ForConstantBuffer(sizeof(CameraPass));
                // TODO: This is what I'm saying it should added as part of the context
                cameraBuffer = OpenGLDriver::GetCurrentDriver()->GetRenderContext()->CreateBuffer(settings);
                cameraBuffer->SetName(cameraKey);
                OpenGLDriver::GetCurrentDriver()->GetObjectRegistry()->AddObject(cameraBuffer);
            }
            RHI::SubResource resource(&cameraPass, {});
            cameraBuffer->SetData(resource);
            
            Ref<RHI::Buffer> skyboxCameraBuffer;
            std::string skyboxCameraKey = "queue-skybox-camera-" + cameraComponent->GetID().ToString();
            
            if (OpenGLDriver::GetCurrentDriver()->GetObjectRegistry()->HasKey(skyboxCameraKey)) {
                skyboxCameraBuffer = OpenGLDriver::GetCurrentDriver()->GetObjectRegistry()->Get<RHI::OpenGLBuffer>(skyboxCameraKey);
            } else {
                RHI::BufferSettings settings = RHI::BufferSettings::ForConstantBuffer(sizeof(CameraPass));
                // TODO: This is what I'm saying it should added as part of the context
                skyboxCameraBuffer = OpenGLDriver::GetCurrentDriver()->GetRenderContext()->CreateBuffer(settings);
                skyboxCameraBuffer->SetName(skyboxCameraKey);
                OpenGLDriver::GetCurrentDriver()->GetObjectRegistry()->AddObject(skyboxCameraBuffer);
            }
            RHI::SubResource skyboxResource(&skyBoxCameraPass, {});
            skyboxCameraBuffer->SetData(skyboxResource);
            
            std::vector<Ref<Shader>> sorted_elements(commandContext->shaders.begin(), commandContext->shaders.end());
            
            std::sort(sorted_elements.begin(), sorted_elements.end(), [](Ref<Shader> shaderA, Ref<Shader> shaderB) {
                return shaderA->GetSubShader().GetPriority() < shaderB->GetSubShader().GetPriority();
            });
            for(const Ref<Shader>& shaderAsset: sorted_elements) {
                Ref<HLSLOpenGLShader> metalShader = DynamicAs<HLSLOpenGLShader>(shaderAsset->GetInternalShader());
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
                        
                        if (OpenGLDriver::GetCurrentDriver()->GetObjectRegistry()->HasKey(materialKey)) {
                            materialBuffer = OpenGLDriver::GetCurrentDriver()->GetObjectRegistry()->Get<RHI::OpenGLBuffer>(materialKey);
                        } else {
                            RHI::BufferSettings settings = RHI::BufferSettings::ForConstantBuffer((uint32_t)material->GetSize());
                            // TODO: This is what I'm saying it should added as part of the context
                            materialBuffer = OpenGLDriver::GetCurrentDriver()->GetRenderContext()->CreateBuffer(settings);
                            materialBuffer->SetName(materialKey);
                            OpenGLDriver::GetCurrentDriver()->GetObjectRegistry()->AddObject(materialBuffer);
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
                        
                        Ref<RHI::OpenGLBuffer> indexBuffer;
                        Ref<RHI::OpenGLBufferSet> vertexBufferSet;
                        
                        if (OpenGLDriver::GetCurrentDriver()->GetObjectRegistry()->HasKey("vertexSet-" + entity.mesh->GetID().ToString())) {
                            vertexBufferSet = OpenGLDriver::GetCurrentDriver()->GetObjectRegistry()->Get<RHI::OpenGLBufferSet>("vertexSet-" + entity.mesh->GetID().ToString());
                        } else {
                            std::vector<Ref<RHI::Buffer>> buffers;
                            std::vector<float> vertices = entity.mesh->GetNativeVector();
                            
                            if (vertices.size() != 0) {
                                RHI::BufferSettings vertexSettings = RHI::BufferSettings::ForVertextBuffer((uint32_t)vertices.size() * sizeof(float));
                                Ref<RHI::Buffer> vertexBuffer = OpenGLDriver::GetCurrentDriver()->GetRenderContext()->CreateBuffer(vertexSettings);
                                RHI::SubResource resourceVertices(vertices.data(), {});
                                vertexBuffer->SetData(resourceVertices);
                                buffers.push_back(vertexBuffer);
                            }
                            vertexBufferSet = DynamicAs<RHI::OpenGLBufferSet>(OpenGLDriver::GetCurrentDriver()->GetRenderContext()->CreateBufferSet(buffers));
                            vertexBufferSet->SetName("vertexSet-" + entity.mesh->GetID().ToString());
                            OpenGLDriver::GetCurrentDriver()->GetObjectRegistry()->AddObject(vertexBufferSet);
                        }
                        
                        if (OpenGLDriver::GetCurrentDriver()->GetObjectRegistry()->HasKey("index-" + entity.mesh->GetID().ToString())) {
                            indexBuffer = OpenGLDriver::GetCurrentDriver()->GetObjectRegistry()->Get<RHI::OpenGLBuffer>("index-" + entity.mesh->GetID().ToString());
                        } else {
                            std::vector<uint32_t> triangles = entity.mesh->GetTriangles();
                            RHI::BufferSettings indexSettings = RHI::BufferSettings::ForIndexBuffer((uint32_t)triangles.size(), RHI::PixelFormat::R32Uint);
                            indexBuffer = DynamicAs<RHI::OpenGLBuffer>(OpenGLDriver::GetCurrentDriver()->GetRenderContext()->CreateBuffer(indexSettings));
                            RHI::SubResource resourceIndices(triangles.data(), {});
                            indexBuffer->SetData(resourceIndices);
                            indexBuffer->SetName("index-" + entity.mesh->GetID().ToString());
                            OpenGLDriver::GetCurrentDriver()->GetObjectRegistry()->AddObject(indexBuffer);
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
                                    
                                    if (OpenGLDriver::GetCurrentDriver()->GetObjectRegistry()->HasKey(objectKey)) {
                                        objectBuffer = OpenGLDriver::GetCurrentDriver()->GetObjectRegistry()->Get<RHI::OpenGLBuffer>(objectKey);
                                    } else {
                                        RHI::BufferSettings settings = RHI::BufferSettings::ForConstantBuffer((uint32_t)constantArgument->GetSize());
                                        // TODO: This is what I'm saying it should added as part of the context
                                        objectBuffer = OpenGLDriver::GetCurrentDriver()->GetRenderContext()->CreateBuffer(settings);
                                        objectBuffer->SetName(objectKey);
                                        OpenGLDriver::GetCurrentDriver()->GetObjectRegistry()->AddObject(objectBuffer);
                                    }
                                    Matrix4x4 object = entity.transform;
                                    RHI::SubResource resource(&object, {});
                                    objectBuffer->SetData(resource);
                                    parameters[value] = objectBuffer->GetResourceView();
                                }
                                if ((std::string("cbPer") + Light::LightName) == key) {
                                    Ref<RHI::Buffer> lightsBuffer;
                                    std::string ligthsKey = "queue-lights";
                                    
                                    if (OpenGLDriver::GetCurrentDriver()->GetObjectRegistry()->HasKey(ligthsKey)) {
                                        lightsBuffer = OpenGLDriver::GetCurrentDriver()->GetObjectRegistry()->Get<RHI::OpenGLBuffer>(ligthsKey);
                                    } else {
                                        RHI::BufferSettings settings = RHI::BufferSettings::ForConstantBuffer((uint32_t)constantArgument->GetSize());
                                        // TODO: This is what I'm saying it should added as part of the context
                                        lightsBuffer = OpenGLDriver::GetCurrentDriver()->GetRenderContext()->CreateBuffer(settings);
                                        lightsBuffer->SetName(ligthsKey);
                                        OpenGLDriver::GetCurrentDriver()->GetObjectRegistry()->AddObject(lightsBuffer);
                                    }
                                    RHI::SubResource resource(lights.data(), {});
                                    lightsBuffer->SetData(resource);
                                    parameters[value] = lightsBuffer->GetResourceView();
                                }
                                if ((std::string("cbPer") + Light::LightSpaceName) == key) {
                                    Ref<RHI::Buffer> lightsSpacesBuffer;
                                    std::string lightsSpacesKey = "queue-lights-spaces";
                                    
                                    if (OpenGLDriver::GetCurrentDriver()->GetObjectRegistry()->HasKey(lightsSpacesKey)) {
                                        lightsSpacesBuffer = OpenGLDriver::GetCurrentDriver()->GetObjectRegistry()->Get<RHI::OpenGLBuffer>(lightsSpacesKey);
                                    } else {
                                        RHI::BufferSettings settings = RHI::BufferSettings::ForConstantBuffer((uint32_t)constantArgument->GetSize());
                                        // TODO: This is what I'm saying it should added as part of the context
                                        lightsSpacesBuffer = OpenGLDriver::GetCurrentDriver()->GetRenderContext()->CreateBuffer(settings);
                                        lightsSpacesBuffer->SetName(lightsSpacesKey);
                                        OpenGLDriver::GetCurrentDriver()->GetObjectRegistry()->AddObject(lightsSpacesBuffer);
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
                                        Ref<OpenGLCubemap> cubemap = DynamicAs<OpenGLCubemap>(assetCubemap->GetInnerTexture());
                                        rhiTexture = cubemap->GetRHITexture();
                                    }
                                    if (texArgument->GetTextureDimensionType() == RHI::TextureDimensionType::Tex2D) {
                                        Ref<Texture2D> assetTexture = DynamicAs<Texture2D>(material->GetTexture(value->GetName()));
                                        Ref<OpenGLTexture2D> texture2D = DynamicAs<OpenGLTexture2D>(assetTexture->GetInnerTexture());
                                        rhiTexture = texture2D->GetRHITexture();
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
                        
                        BindToRenderTexture();
                        
                        cmdList->DrawIndexed(
                                             GetTopologyEx(entity.mesh->GetSubMesh(entity.subMeshIndex).GetTopology()),
                                             indexBuffer->GetSettings().GetCount()
                                             );
                        
                        UnbindToRenderTexture();
                        commandIndex++;
                    }
                }
            }
        }
        
        
//        for(Ref<GraphicsCommand> command: GraphicsContext::GetList().GetCommands()) {
//            Ref<DrawMeshCommand> drawMeshCommand = std::dynamic_pointer_cast<DrawMeshCommand>(command);
//            
//            Ref<Mesh> mesh1 = drawMeshCommand->GetMesh();
//            Ref<Material> material = drawMeshCommand->GetMaterial();
//            Ref<Camera> camera = drawMeshCommand->GetCamera()->GetCamera();
//            Matrix4x4 transform = drawMeshCommand->GetTransform();
//            
//            if (mesh1 == nullptr || mesh1->GetVertices().size() == 1) continue; // It's a light
//            
//            Camera* cam = camera.get();
//            
//            bool isSkybox = Shader::GetShader(drawMeshCommand->GetMaterial()->GetShaderName())->GetSubShader().GetPriority() < 200;
//            TransformComponent& cameraTransform = drawMeshCommand->GetCamera()->GetGameObject()->GetTransform();
//            
//            Vector3 focalPoint = !isSkybox ? cam->GetFocalPoint() : VWolf::Vector3(0.0f, 0.0f, 0.0f);
//            float zoom = !isSkybox ? cam->GetZoom() : 1.0f;
//            Vector3 position = Vector3::Zero;
//            if ((drawMeshCommand->GetCamera()->GetGameObject()->GetFlags() & HideFlags::Editor) == HideFlags::Editor)
//                position = focalPoint - cameraTransform.GetPosition() * zoom;
//            else {
//                if (isSkybox)
//                    position = Vector3::Zero;
//                else
//                    position = cameraTransform.GetPosition();
//            }
//            
//            Matrix4x4 viewMatrix = cam->CalculateView(position,
//                                                      Quaternion::Euler(
//                                                                        cameraTransform.GetEulerAngles().GetX(),
//                                                                        cameraTransform.GetEulerAngles().GetY(),
//                                                                        cameraTransform.GetEulerAngles().GetZ()
//                                                                        ));
//
//            CameraPass cameraPass = {
//                viewMatrix,
//                viewMatrix.GetInverse(),
//                cam->GetProjection(),
//                cam->GetProjection().GetInverse(),
//                cam->GetViewProjection(viewMatrix),
//                cam->GetViewProjection(viewMatrix).GetInverse(),
//                cameraTransform.GetPosition(),
//                0,
//                cam->GetDisplaySize(),
//                { 1 / cam->GetDisplaySize().GetX(), 1 / cam->GetDisplaySize().GetY() },
//                cam->GetNearZ(),
//                cam->GetFarZ(),
//                Time::GetTotalTime(),
//                Time::GetDeltaTime()
//            };
//
//            Ref<HLSLOpenGLShader> shader = std::dynamic_pointer_cast<HLSLOpenGLShader>(Shader::GetShader(material->GetShaderName().c_str())->GetInternalShader());
//            void* material1 = material->GetDataPointer();
//            Light* lights = GraphicsContext::GetLights().data();
//            std::vector<ShaderInput> textures = shader->GetTextureInputs();
//            shader->Bind();
//            for (GLuint index = 0; index < textures.size(); index++) {
//                if (textures[index].GetName() == "Shadow") {
//                    shadowMap->DepthTextureBind(index);
//                    GLThrowIfFailed(glUniform1i(textures[index].GetIndex(), index));
//                } else {
//                    Texture* tex = material->GetTexture(textures[index].GetName()).get();
//                    BaseBindable* texture;
//                    if (dynamic_cast<Texture2D *>(tex) != nullptr) {
//                        texture = dynamic_cast<BaseBindable*>(dynamic_cast<Texture2D *>(tex)->GetInnerTexture().get());
//                    }
//                    else if (dynamic_cast<Cubemap *>(tex) != nullptr) {
//                        texture = dynamic_cast<BaseBindable*>(dynamic_cast<Cubemap *>(tex)->GetInnerTexture().get());
//                    }
//                    else { continue; }
//                    if (texture != nullptr) {
//                        texture->Bind(index);
//                        // TODO: Move this inside bind
//                        GLThrowIfFailed(glUniform1i(textures[index].GetIndex(), index));
//                    }
//                }
//            }
//            shader->SetData(&cameraPass, Shader::CameraBufferName, sizeof(CameraPass), 0);
//            shader->SetData(&transform, Shader::ObjectBufferName, sizeof(Matrix4x4), 0);
//            shader->SetData(material1, materialName.c_str(), material->GetSize(), 0);
//            if (lights) {
//                shader->SetData(lights, Light::LightName, sizeof(Light) * Light::LightsMax, 0);
//                Matrix4x4* spacesPointer = GraphicsContext::GetLightsSpaces().data();
//                shader->SetData(spacesPointer, Light::LightSpaceName, sizeof(Matrix4x4) * Light::LightsMax, 0);
//            }
//            mesh1->BuildVertexBuffer(shader->GetAttributes());
//            
//            auto data = mesh1->GetNativeVector();
//            auto indices = mesh1->GetTriangles();
//            Ref<OpenGLVertexBuffer> vertices = CreateRef<OpenGLVertexBuffer>(data.data(), data.size() * sizeof(float));
//            Ref<OpenGLIndexBuffer> index = CreateRef<OpenGLIndexBuffer>(indices.data(), indices.size());
//            Ref<OpenGLVertexArray> group = CreateRef<OpenGLVertexArray>(vertices, shader->GetAttributes());
//            
//            group->Bind();
//            vertices->Bind();
//            index->Bind();
//            
//            uint32_t count = index->GetCount();
//            BindToRenderTexture();
//            GLThrowIfFailed(glDrawElements(GetTopology(mesh1->GetSubMesh(0).GetTopology()), count, GL_UNSIGNED_INT, nullptr));
//            UnbindToRenderTexture();
//            vertices->Unbind();
//            index->Unbind();
//            group->Unbind();
//            for (GLuint index = 0; index < textures.size(); index++) {
//                if (textures[index].GetName() == "Shadow") {
//                    shadowMap->DepthTextureUnbind(index);
//                } else {
//                    Texture* tex = material->GetTexture(textures[index].GetName()).get();
//                    BaseBindable* texture;
//                    if (dynamic_cast<Texture2D *>(tex) != nullptr) {
//                        texture = dynamic_cast<BaseBindable*>(dynamic_cast<Texture2D *>(tex)->GetInnerTexture().get());
//                    }
//                    else if (dynamic_cast<Cubemap *>(tex) != nullptr) {
//                        texture = dynamic_cast<BaseBindable*>(dynamic_cast<Cubemap *>(tex)->GetInnerTexture().get());
//                    }
//                    else { continue; }
//                    if (texture != nullptr) {
//                        texture->Unbind(index);
//                    }
//                }
//            }
//            shader->Unbind();
//            
//            free(material1);
//        }
    }

    void OpenGLGraphics::DrawPostProcess() {
        
    }
}
