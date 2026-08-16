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

#include "VWolf/Core/Render/GraphicsContext.h"

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
        shadowMap = CreateRef<MetalRenderTexture>(2048, 2048, true);
        
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
        
        constantBufferIndexPerShader.clear();
        itemsCount = 0;
        
        ClearColorImpl(GraphicsContext::GetBackgroundColor());

        DrawShadowMap();
        DrawQueue();
        DrawPostProcess();
        
    }

    void MetalGraphics::DrawShadowMap() {
        Ref<RHI::MetalRenderCommandList> cmdList = shadowMap->GetCommandList();

        HLSLMetalShader* metalShader = (HLSLMetalShader*)Shader::GetShader("Shadow")->GetInternalShader().get();
        cmdList->SetRenderState(metalShader->GetState());
        
        for (size_t index = 0; index < GraphicsContext::GetLights().size(); index++) {
            
            uint32_t commandIndex = 0;
            
            for(Ref<GraphicsCommand> command: GraphicsContext::GetList().GetCommands()) {
                Ref<DrawMeshCommand> drawMeshCommand = std::dynamic_pointer_cast<DrawMeshCommand>(command);
                
                // It's a light
                if (drawMeshCommand->GetMesh() == nullptr || drawMeshCommand->GetMesh()->GetVertices().size() == 1) continue;
                
                if (drawMeshCommand->GetCastShadows()) {
                    
                    drawMeshCommand->GetMesh()->BuildVertexBuffer(metalShader->GetAttributes());
                    
                    Ref<RHI::MetalBuffer> indexBuffer;
                    Ref<RHI::MetalBufferSet> vertexBufferSet;
                    
                    if (MetalDriver::GetCurrent()->GetObjectRegistry()->HasKey("vertexSet-" + drawMeshCommand->GetMesh()->GetID().ToString())) {
                        vertexBufferSet = MetalDriver::GetCurrent()->GetObjectRegistry()->Get<RHI::MetalBufferSet>("vertexSet-" + drawMeshCommand->GetMesh()->GetID().ToString());
                    } else {
                        std::vector<Ref<RHI::Buffer>> buffers;
                        std::vector<float> vertices = drawMeshCommand->GetMesh()->GetNativeVector();
                        
                        if (vertices.size() != 0) {
                            RHI::BufferSettings vertexSettings = RHI::BufferSettings::ForVertextBuffer((uint32_t)vertices.size() * sizeof(float));
                            Ref<RHI::Buffer> vertexBuffer = MetalDriver::GetCurrent()->GetRenderContext()->CreateBuffer(vertexSettings);
                            RHI::SubResource resourceVertices(vertices.data(), {});
                            vertexBuffer->SetData(resourceVertices);
                            buffers.push_back(vertexBuffer);
                        }
                        vertexBufferSet = DynamicAs<RHI::MetalBufferSet>(MetalDriver::GetCurrent()->GetRenderContext()->CreateBufferSet(buffers));
                        vertexBufferSet->SetName("vertexSet-" + drawMeshCommand->GetMesh()->GetID().ToString());
                        MetalDriver::GetCurrent()->GetObjectRegistry()->AddObject(vertexBufferSet);
                    }
                    
                    if (MetalDriver::GetCurrent()->GetObjectRegistry()->HasKey("index-" + drawMeshCommand->GetMesh()->GetID().ToString())) {
                        indexBuffer = MetalDriver::GetCurrent()->GetObjectRegistry()->Get<RHI::MetalBuffer>("index-" + drawMeshCommand->GetMesh()->GetID().ToString());
                    } else {
                        std::vector<uint32_t> triangles = drawMeshCommand->GetMesh()->GetTriangles();
                        RHI::BufferSettings indexSettings = RHI::BufferSettings::ForIndexBuffer((uint32_t)triangles.size(), RHI::PixelFormat::R32Uint);
                        indexBuffer = DynamicAs<RHI::MetalBuffer>(MetalDriver::GetCurrent()->GetRenderContext()->CreateBuffer(indexSettings));
                        RHI::SubResource resourceIndices(triangles.data(), {});
                        indexBuffer->SetData(resourceIndices);
                        indexBuffer->SetName("index-" + drawMeshCommand->GetMesh()->GetID().ToString());
                        MetalDriver::GetCurrent()->GetObjectRegistry()->AddObject(indexBuffer);
                    }
                    
                    objectTransforms[drawMeshCommand->GetMesh()->GetID()] = drawMeshCommand->GetTransform();
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
                   
                    cmdList->SetVertexBuffers(vertexBufferSet);
                    cmdList->SetIndexBuffer(indexBuffer);
                    // TODO: Temporary
                    std::map<std::string, Ref<RHI::BindableProgramArgument>> arguments = metalShader->GetProgram()->GetSettings().GetProgramArguments();
                    std::unordered_map<Ref<RHI::BindableProgramArgument>, RHI::ProgramArgumentBindingValue, RHI::BindableProgramArgument::Hash> parameters;
                    for(const auto& [key, value]: arguments) {
                        
                        if (value->GetResourceType() == RHI::ResourceType::Buffer) {
                            Ref<RHI::ConstantBufferProgramArgument> constantArgument = DynamicAs<RHI::ConstantBufferProgramArgument>(value);
                            std::stringstream ss;
                            Ref<RHI::Buffer> buffer;
                            
                            if ((std::string("cbPer") + Shader::CameraBufferName) == key) {
                                
                                ss << (std::string("shadow-cbPer") + Shader::CameraBufferName);
                                ss << "-";
                                ss << commandIndex;
                                std::string finalKey = ss.str();
                                
                                const Matrix4x4& viewProjection = GraphicsContext::GetLightsSpaces()[index];
                                
                                if (MetalDriver::GetCurrent()->GetObjectRegistry()->HasKey(finalKey)) {
                                    buffer = MetalDriver::GetCurrent()->GetObjectRegistry()->Get<RHI::MetalBuffer>(finalKey);
                                } else {
                                    RHI::BufferSettings settings = RHI::BufferSettings::ForConstantBuffer((uint32_t)constantArgument->GetSize());
                                    // TODO: This is what I'm saying it should added as part of the context
                                    buffer = MetalDriver::GetCurrent()->GetRenderContext()->CreateBuffer(settings);
                                    buffer->SetName(finalKey);
                                    MetalDriver::GetCurrent()->GetObjectRegistry()->AddObject(indexBuffer);
                                }
                                
                                RHI::SubResource resource(&viewProjection, {});
                                buffer->SetData(resource);
                                parameters[value] = buffer->GetResourceView();
                            }
                            if ((std::string("cbPer") + Shader::ObjectBufferName) == key) {
                                
                                ss << (std::string("shadow-cbPer") + Shader::ObjectBufferName);
                                ss << "-";
                                ss << commandIndex;
                                std::string finalKey = ss.str();
                                
                                const Matrix4x4& objectMatrix = objectTransforms[drawMeshCommand->GetMesh()->GetID()];
                                
                                if (MetalDriver::GetCurrent()->GetObjectRegistry()->HasKey(finalKey)) {
                                    buffer = MetalDriver::GetCurrent()->GetObjectRegistry()->Get<RHI::MetalBuffer>(finalKey);
                                } else {
                                    RHI::BufferSettings settings = RHI::BufferSettings::ForConstantBuffer((uint32_t)constantArgument->GetSize());
                                    // TODO: This is what I'm saying it should added as part of the context
                                    buffer = MetalDriver::GetCurrent()->GetRenderContext()->CreateBuffer(settings);
                                    buffer->SetName(finalKey);
                                    MetalDriver::GetCurrent()->GetObjectRegistry()->AddObject(indexBuffer);
                                }
                                
                                RHI::SubResource resource(&objectMatrix, {});
                                buffer->SetData(resource);
                                parameters[value] = buffer->GetResourceView();
                            }
                        }
                    }
                    
                    RHI::ProgramArgumentBindings bindings(
                                                          metalShader->GetProgram(), parameters);
                    //
                    cmdList->SetProgramBindings(bindings);

                    cmdList->DrawIndexed(
                                         GetTopologyEx(drawMeshCommand->GetMesh()->GetSubMesh(drawMeshCommand->GetSubmeshIndex()).GetTopology()),
                                         indexBuffer->GetSettings().GetCount()
                                         );
                    commandIndex++;
                }
            }
        }
        
        shadowMap->Commit();
    }

    void MetalGraphics::DrawQueue() {
        
        Ref<RHI::MetalRenderCommandList> cmdList = commandList;
        if (m_p_renderTexture) {
            cmdList = ((MetalRenderTexture*)m_p_renderTexture.get())->GetCommandList();
        }

        // TODO: Do we really need this?
        if (GraphicsContext::GetLights().size() == 0) {
            GraphicsContext::GetLights().push_back(Light());
        }
        if (GraphicsContext::GetLightsSpaces().size() == 0) {
            GraphicsContext::GetLightsSpaces().push_back(Matrix4x4());
        }
        Light* lights = GraphicsContext::GetLights().data();
        Matrix4x4* spacesPointer = GraphicsContext::GetLightsSpaces().data();

        Camera* cam = nullptr;

        uint32_t commandIndex = 0;
        for(Ref<GraphicsCommand> command: GraphicsContext::GetList().GetCommands()) {
            Ref<DrawMeshCommand> drawMeshCommand = std::dynamic_pointer_cast<DrawMeshCommand>(command);
    
            int shapes = constantBufferIndexPerShader.count(drawMeshCommand->GetMaterial()->GetShaderName()) > 0 ? constantBufferIndexPerShader[drawMeshCommand->GetMaterial()->GetShaderName()]: 0;

            if (drawMeshCommand->GetMesh() == nullptr || drawMeshCommand->GetMesh()->GetVertices().size() == 1) continue; // It's a light
            
            cam = drawMeshCommand->GetCamera()->GetCamera().get();

            bool isSkybox = Shader::GetShader(drawMeshCommand->GetMaterial()->GetShaderName())->GetSubShader().GetPriority() < 200;
            TransformComponent& cameraTransform = drawMeshCommand->GetCamera()->GetGameObject()->GetTransform();
            
            Vector3 focalPoint = !isSkybox ? cam->GetFocalPoint() : VWolf::Vector3(0.0f, 0.0f, 0.0f);
            float zoom = !isSkybox ? cam->GetZoom() : 1.0f;
            
            Vector3 position = Vector3::Zero;
            if ((drawMeshCommand->GetCamera()->GetGameObject()->GetFlags() & HideFlags::Editor) == HideFlags::Editor)
                position = focalPoint - cameraTransform.GetPosition() * zoom;
            else {
                if (isSkybox)
                    position = Vector3::Zero;
                else
                    position = cameraTransform.GetPosition();
            }
            
            Matrix4x4 viewMatrix = cam->CalculateView(position,
                                                      Quaternion::Euler(
                                                                        cameraTransform.GetEulerAngles().GetX(),
                                                                        cameraTransform.GetEulerAngles().GetY(),
                                                                        cameraTransform.GetEulerAngles().GetZ()
                                                                        ));

            CameraPass cameraPass = {
                viewMatrix,
                viewMatrix.GetInverse(),
                cam->GetProjection(),
                cam->GetProjection().GetInverse(),
                cam->GetViewProjection(viewMatrix),
                cam->GetViewProjection(viewMatrix).GetInverse(),
                cameraTransform.GetPosition(),
                0,
                cam->GetDisplaySize(),
                { 1 / cam->GetDisplaySize().GetX(), 1 / cam->GetDisplaySize().GetY() },
                cam->GetNearZ(),
                cam->GetFarZ(),
                Time::GetTotalTime(),
                Time::GetDeltaTime()
            };
            
            Ref<HLSLMetalShader> metalShader = std::dynamic_pointer_cast<HLSLMetalShader>(Shader::GetShader(drawMeshCommand->GetMaterial()->GetShaderName())->GetInternalShader());

            drawMeshCommand->GetMesh()->BuildVertexBuffer(metalShader->GetAttributes());
            
            Ref<RHI::MetalBuffer> indexBuffer;
            Ref<RHI::MetalBufferSet> vertexBufferSet;
            
            if (MetalDriver::GetCurrent()->GetObjectRegistry()->HasKey("vertexSet-" + drawMeshCommand->GetMesh()->GetID().ToString())) {
                vertexBufferSet = MetalDriver::GetCurrent()->GetObjectRegistry()->Get<RHI::MetalBufferSet>("vertexSet-" + drawMeshCommand->GetMesh()->GetID().ToString());
            } else {
                std::vector<Ref<RHI::Buffer>> buffers;
                std::vector<float> vertices = drawMeshCommand->GetMesh()->GetNativeVector();
                
                if (vertices.size() != 0) {
                    RHI::BufferSettings vertexSettings = RHI::BufferSettings::ForVertextBuffer((uint32_t)vertices.size() * sizeof(float));
                    Ref<RHI::Buffer> vertexBuffer = MetalDriver::GetCurrent()->GetRenderContext()->CreateBuffer(vertexSettings);
                    RHI::SubResource resourceVertices(vertices.data(), {});
                    vertexBuffer->SetData(resourceVertices);
                    buffers.push_back(vertexBuffer);
                }
                vertexBufferSet = DynamicAs<RHI::MetalBufferSet>(MetalDriver::GetCurrent()->GetRenderContext()->CreateBufferSet(buffers));
                vertexBufferSet->SetName("vertexSet-" + drawMeshCommand->GetMesh()->GetID().ToString());
                MetalDriver::GetCurrent()->GetObjectRegistry()->AddObject(vertexBufferSet);
            }
            
            if (MetalDriver::GetCurrent()->GetObjectRegistry()->HasKey("index-" + drawMeshCommand->GetMesh()->GetID().ToString())) {
                indexBuffer = MetalDriver::GetCurrent()->GetObjectRegistry()->Get<RHI::MetalBuffer>("index-" + drawMeshCommand->GetMesh()->GetID().ToString());
            } else {
                std::vector<uint32_t> triangles = drawMeshCommand->GetMesh()->GetTriangles();
                RHI::BufferSettings indexSettings = RHI::BufferSettings::ForIndexBuffer((uint32_t)triangles.size(), RHI::PixelFormat::R32Uint);
                indexBuffer = DynamicAs<RHI::MetalBuffer>(MetalDriver::GetCurrent()->GetRenderContext()->CreateBuffer(indexSettings));
                RHI::SubResource resourceIndices(triangles.data(), {});
                indexBuffer->SetData(resourceIndices);
                indexBuffer->SetName("index-" + drawMeshCommand->GetMesh()->GetID().ToString());
                MetalDriver::GetCurrent()->GetObjectRegistry()->AddObject(indexBuffer);
            }
            
            objectTransforms[drawMeshCommand->GetMesh()->GetID()] = drawMeshCommand->GetTransform();

            void* material1 = drawMeshCommand->GetMaterial()->GetDataPointer();
           
            cmdList->SetRenderState(metalShader->GetState());
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

            if (vertexBufferSet->GetBuffers().size() > 0) {
                cmdList->SetVertexBuffers(vertexBufferSet);
            }
            cmdList->SetIndexBuffer(indexBuffer);
            
            // TODO: Temporary
            std::map<std::string, Ref<RHI::BindableProgramArgument>> arguments = metalShader->GetProgram()->GetSettings().GetProgramArguments();
            std::unordered_map<Ref<RHI::BindableProgramArgument>, RHI::ProgramArgumentBindingValue, RHI::BindableProgramArgument::Hash> parameters;
            for(const auto& [key, value]: arguments) {
                
                if (value->GetResourceType() == RHI::ResourceType::Buffer) {
                    Ref<RHI::ConstantBufferProgramArgument> constantArgument = DynamicAs<RHI::ConstantBufferProgramArgument>(value);
                    std::stringstream ss;
                    Ref<RHI::Buffer> buffer;
                    
                    if ((std::string("cbPer") + Shader::CameraBufferName) == key) {
                        
                        ss << (std::string("queue-cbPer") + Shader::CameraBufferName);
                        ss << "-";
                        ss << commandIndex;
                        std::string finalKey = ss.str();
                        
                        if (MetalDriver::GetCurrent()->GetObjectRegistry()->HasKey(finalKey)) {
                            buffer = MetalDriver::GetCurrent()->GetObjectRegistry()->Get<RHI::MetalBuffer>(finalKey);
                        } else {
                            RHI::BufferSettings settings = RHI::BufferSettings::ForConstantBuffer((uint32_t)constantArgument->GetSize());
                            // TODO: This is what I'm saying it should added as part of the context
                            buffer = MetalDriver::GetCurrent()->GetRenderContext()->CreateBuffer(settings);
                            buffer->SetName(finalKey);
                            MetalDriver::GetCurrent()->GetObjectRegistry()->AddObject(buffer);
                        }
                        
                        RHI::SubResource resource(&cameraPass, {});
                        buffer->SetData(resource);
                        parameters[value] = buffer->GetResourceView();
                    }
                    if ((std::string("cbPer") + Shader::ObjectBufferName) == key) {
                        
                        ss << (std::string("queue-cbPer") + Shader::ObjectBufferName);
                        ss << "-";
                        ss << commandIndex;
                        std::string finalKey = ss.str();
                        
                        const Matrix4x4& objectMatrix = objectTransforms[drawMeshCommand->GetMesh()->GetID()];
                        
                        if (MetalDriver::GetCurrent()->GetObjectRegistry()->HasKey(finalKey)) {
                            buffer = MetalDriver::GetCurrent()->GetObjectRegistry()->Get<RHI::MetalBuffer>(finalKey);
                        } else {
                            RHI::BufferSettings settings = RHI::BufferSettings::ForConstantBuffer((uint32_t)constantArgument->GetSize());
                            // TODO: This is what I'm saying it should added as part of the context
                            buffer = MetalDriver::GetCurrent()->GetRenderContext()->CreateBuffer(settings);
                            buffer->SetName(finalKey);
                            MetalDriver::GetCurrent()->GetObjectRegistry()->AddObject(buffer);
                        }
                        RHI::SubResource resource(&objectMatrix, {});
                        buffer->SetData(resource);
                        parameters[value] = buffer->GetResourceView();
                    }
                    if ((std::string("cbPer") + materialName) == key) {
                        
                        ss << (std::string("queue-cbPer") + materialName);
                        ss << "-";
                        ss << commandIndex;
                        std::string finalKey = ss.str();
                        
                        if (MetalDriver::GetCurrent()->GetObjectRegistry()->HasKey(finalKey)) {
                            buffer = MetalDriver::GetCurrent()->GetObjectRegistry()->Get<RHI::MetalBuffer>(finalKey);
                        } else {
                            RHI::BufferSettings settings = RHI::BufferSettings::ForConstantBuffer((uint32_t)constantArgument->GetSize());
                            // TODO: This is what I'm saying it should added as part of the context
                            buffer = MetalDriver::GetCurrent()->GetRenderContext()->CreateBuffer(settings);
                            buffer->SetName(finalKey);
                            MetalDriver::GetCurrent()->GetObjectRegistry()->AddObject(buffer);
                        }
                        
                        RHI::SubResource resource(material1, {});
                        buffer->SetData(resource);
                        parameters[value] = buffer->GetResourceView();
                    }
                    if ((std::string("cbPer") + Light::LightName) == key) {
                        
                        ss << (std::string("queue-cbPer") + Light::LightName);
                        ss << "-";
                        ss << commandIndex;
                        std::string finalKey = ss.str();
                        
                        if (MetalDriver::GetCurrent()->GetObjectRegistry()->HasKey(finalKey)) {
                            buffer = MetalDriver::GetCurrent()->GetObjectRegistry()->Get<RHI::MetalBuffer>(finalKey);
                        } else {
                            RHI::BufferSettings settings = RHI::BufferSettings::ForConstantBuffer((uint32_t)constantArgument->GetSize());
                            // TODO: This is what I'm saying it should added as part of the context
                            buffer = MetalDriver::GetCurrent()->GetRenderContext()->CreateBuffer(settings);
                            buffer->SetName(finalKey);
                            MetalDriver::GetCurrent()->GetObjectRegistry()->AddObject(buffer);
                        }
                        RHI::SubResource resource(lights, {});
                        buffer->SetData(resource);
                        parameters[value] = buffer->GetResourceView();
                    }
                    if ((std::string("cbPer") + Light::LightSpaceName) == key) {
                        
                        ss << (std::string("queue-cbPer") + Light::LightSpaceName);
                        ss << "-";
                        ss << commandIndex;
                        std::string finalKey = ss.str();
                        
                        if (MetalDriver::GetCurrent()->GetObjectRegistry()->HasKey(finalKey)) {
                            buffer = MetalDriver::GetCurrent()->GetObjectRegistry()->Get<RHI::MetalBuffer>(finalKey);
                        } else {
                            RHI::BufferSettings settings = RHI::BufferSettings::ForConstantBuffer((uint32_t)constantArgument->GetSize());
                            // TODO: This is what I'm saying it should added as part of the context
                            buffer = MetalDriver::GetCurrent()->GetRenderContext()->CreateBuffer(settings);
                            buffer->SetName(finalKey);
                            MetalDriver::GetCurrent()->GetObjectRegistry()->AddObject(buffer);
                        }
                        RHI::SubResource resource(spacesPointer, {});
                        buffer->SetData(resource);
                        parameters[value] = buffer->GetResourceView();
                    }
                } else if (value->GetResourceType() == RHI::ResourceType::Texture) {
                    Ref<RHI::TextureProgramArgument> texArgument = DynamicAs<RHI::TextureProgramArgument>(value);
                    Ref<RHI::Texture> rhiTexture;
                    if (value->GetName() == "Shadow") {
                        rhiTexture = shadowMap->GetRHIDepthTexture();
                    } else {
                        if (texArgument->GetTextureDimensionType() == RHI::TextureDimensionType::Cube) {
                            Ref<Cubemap> assetCubemap = DynamicAs<Cubemap>(drawMeshCommand->GetMaterial()->GetTexture(value->GetName()));
                            Ref<MetalCubemap> metalCubemap = DynamicAs<MetalCubemap>(assetCubemap->GetInnerTexture());
                            rhiTexture = metalCubemap->GetRHITexture();
                        }
                        if (texArgument->GetTextureDimensionType() == RHI::TextureDimensionType::Tex2D) {
                            Ref<Texture2D> assetTexture = DynamicAs<Texture2D>(drawMeshCommand->GetMaterial()->GetTexture(value->GetName()));
                            Ref<MetalTexture2D> metalTexture2D = DynamicAs<MetalTexture2D>(assetTexture->GetInnerTexture());
                            rhiTexture = metalTexture2D->GetRHITexture();
                        }
                    }
                    parameters[value] = rhiTexture->GetTextureView({});
                }
            }
            
            RHI::ProgramArgumentBindings bindings(
                                                  metalShader->GetProgram(), parameters);
            //
            cmdList->SetProgramBindings(bindings);
            
            cmdList->DrawIndexed(
                                 GetTopologyEx(drawMeshCommand->GetMesh()->GetSubMesh(drawMeshCommand->GetSubmeshIndex()).GetTopology()),
                                 indexBuffer->GetSettings().GetCount()
                                 );
            commandIndex++;
            free(material1);
            itemsCount++;
            constantBufferIndexPerShader[drawMeshCommand->GetMaterial()->GetShaderName()] = ++shapes;
        }
    }

    void MetalGraphics::DrawPostProcess() {
        
    }
}

#endif
