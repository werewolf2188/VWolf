//
//  MetalGraphics.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 4/19/24.
//

#include "vwpch.h"

#if defined(VWOLF_PLATFORM_MACOS) || defined(VWOLF_PLATFORM_IOS)
#include "MetalGraphics.h"

#include "MetalShader.h"

#include "VWolf/Core/Render/RenderItem.h"
#include "VWolf/Platform/Metal/MetalDriver.h"
#include "VWolf/Platform/Metal/Render/MetalTexture.h"

#include "VWolf/Core/Debug/ShapeHelper.h"

#include "VWolf/Core/Render/GraphicsContext.h"

namespace VWolf {

    static MTL::PrimitiveType GetTopology(Topology topology) {
        switch (topology) {
            case Topology::Triangles:
                return MTL::PrimitiveType::PrimitiveTypeTriangle;
            case Topology::Quads:
                return MTL::PrimitiveType::PrimitiveTypeTriangleStrip;
            case Topology::Lines:
                return MTL::PrimitiveType::PrimitiveTypeLine;
            case Topology::LinesStrip:
                return MTL::PrimitiveType::PrimitiveTypeLineStrip;
            case Topology::Points:
                return MTL::PrimitiveType::PrimitiveTypePoint;
            default: return MTL::PrimitiveType::PrimitiveTypePoint;
        }
    }

    void MetalGraphics::Initialize() {
        emptyShadowMap = std::dynamic_pointer_cast<MetalTexture2D>(CreateRef<Texture2D>(UUID::NewUUID(), TextureDefault::White, 1024, 1024, TextureOptions())->GetInnerTexture());
        shadowMap = CreateRef<MetalRenderTexture>(2048, 2048, true);
    }

    void MetalGraphics::ClearColorImpl(Color color) {
        MetalDriver::GetCurrent()->GetSurface()->GetRenderPassDescriptor()->colorAttachments()->object(0)->setClearColor(MTL::ClearColor::Make(color.GetR(), color.GetG(), color.GetB(), color.GetA()));
        if (renderTexture) {
            auto metalRenderTexture = (MetalRenderTexture*)renderTexture.get();
            metalRenderTexture->GetRenderPassDescriptor()->colorAttachments()->object(0)->setClearColor(MTL::ClearColor::Make(color.GetR(), color.GetG(), color.GetB(), color.GetA()));
        }
        shadowMap->GetRenderPassDescriptor()->colorAttachments()->object(0)->setClearColor(MTL::ClearColor::Make(color.GetR(), color.GetG(), color.GetB(), color.GetA()));
    }

    void MetalGraphics::ClearImpl() {
        MetalDriver::GetCurrent()->GetSurface()->GetRenderPassDescriptor()->colorAttachments()->object(0)->setLoadAction(MTL::LoadAction::LoadActionClear);
        if (renderTexture) {
            auto metalRenderTexture = (MetalRenderTexture*)renderTexture.get();
            metalRenderTexture->GetRenderPassDescriptor()->colorAttachments()->object(0)->setLoadAction(MTL::LoadAction::LoadActionClear);
        }
        shadowMap->GetRenderPassDescriptor()->colorAttachments()->object(0)->setLoadAction(MTL::LoadAction::LoadActionClear);
    }

    void MetalGraphics::BeginFrameImpl() {
        
    }

    void MetalGraphics::EndFrameImpl() {
        if (renderTexture) {
            ((MetalRenderTexture*)renderTexture.get())->Commit();
        }
        encoder->endEncoding();
        commandBuffer->presentDrawable(MetalDriver::GetCurrent()->GetSurface()->GetCurrentDrawable());
        commandBuffer->commit();
        
        pool->release();
        pool = nullptr;
    }

    void MetalGraphics::SetRenderTextureImpl(Ref<RenderTexture> renderTexture) {
        this->renderTexture = renderTexture->GetInnerTexture();
        
    }

    void MetalGraphics::BeginSceneImpl() {
    }

    void MetalGraphics::EndSceneImpl() {
        pool = NS::AutoreleasePool::alloc()->init();
        commandBuffer =  MetalDriver::GetCurrent()->GetCommand()->GetCommandQueue()->commandBuffer();
        if (renderTexture) {
            ((MetalRenderTexture*)renderTexture.get())->Prepare();
        }
        shadowMap->Prepare();
        MetalDriver::GetCurrent()->GetSurface()->Begin();

        MetalDriver::GetCurrent()->GetSurface()->GetRenderPassDescriptor()->colorAttachments()->object(0)->setTexture(MetalDriver::GetCurrent()->GetSurface()->GetCurrentDrawable()->texture());

        constantBufferIndexPerShader.clear();
        itemsCount = 0;
        if (renderTexture) {
            ((MetalRenderTexture*)renderTexture.get())->StartEncoder();
        }
        
        ClearColorImpl(GraphicsContext::GetBackgroundColor());
        ClearImpl();
        
        encoder = commandBuffer->renderCommandEncoder(MetalDriver::GetCurrent()->GetSurface()->GetRenderPassDescriptor());
        DrawShadowMap();
        DrawQueue();
        DrawPostProcess();
        
    }

    void MetalGraphics::DrawShadowMap() {
        MTL::RenderCommandEncoder* dsvEncoder = shadowMap->StartEncoder();
        MetalShader* metalShader = (MetalShader*)Shader::GetShader("Shadow")->GetInternalShader().get();
        
        for (size_t index = 0; index < GraphicsContext::GetLights().size(); index++) {
            int shadowShapes = 0;
            Matrix4x4& viewProjection = GraphicsContext::GetLightsSpaces()[index];
            
            for(Ref<GraphicsCommand> command: GraphicsContext::GetList().GetCommands()) {
                Ref<DrawMeshCommand> drawMeshCommand = std::dynamic_pointer_cast<DrawMeshCommand>(command);
                
                // It's a light
                if (drawMeshCommand->GetMesh() == nullptr || drawMeshCommand->GetMesh()->GetVertices().size() == 1) continue;
                
                if (drawMeshCommand->GetCastShadows()) {
                    drawMeshCommand->GetMesh()->BuildVertexBuffer(metalShader->GetAttributes());
                    
                    if (shadowShapes >= shadowBufferGroups.size()) {
                        shadowBufferGroups.push_back(CreateRef<MetalBufferGroup>(drawMeshCommand->GetMesh()));
                        shadowObjectTransforms.push_back(drawMeshCommand->GetTransform());
                    } else {
                        shadowBufferGroups[shadowShapes]->SetData(drawMeshCommand->GetMesh());
                        shadowObjectTransforms[shadowShapes] = drawMeshCommand->GetTransform();
                    }
                    
                    metalShader->UseShader(dsvEncoder);
                    metalShader->Bind();
                    metalShader->SetObjectIndex(shadowShapes);
                    metalShader->SetVertexBufferIndex(shadowBufferGroups[shadowShapes]->GetVertexBuffer());
                    metalShader->SetData(&viewProjection, Shader::CameraBufferName, sizeof(Matrix4x4), shadowShapes);
                    metalShader->SetData(&shadowObjectTransforms[shadowShapes], Shader::ObjectBufferName, sizeof(Matrix4x4), shadowShapes);

                    metalShader->Draw(GetTopology(drawMeshCommand->GetMesh()->GetSubMesh(drawMeshCommand->GetSubmeshIndex()).GetTopology()), shadowBufferGroups[shadowShapes]->GetIndexBuffer());
                    shadowShapes++;
                }
            }
        }

        shadowMap->Commit();
    }

    void MetalGraphics::DrawQueue() {
        
        MTL::RenderCommandEncoder* rtvEncoder = nullptr;
        if (renderTexture) {
            rtvEncoder = ((MetalRenderTexture*)renderTexture.get())->StartEncoder();
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

        for(Ref<GraphicsCommand> command: GraphicsContext::GetList().GetCommands()) {
            Ref<DrawMeshCommand> drawMeshCommand = std::dynamic_pointer_cast<DrawMeshCommand>(command);
    
            int shapes = constantBufferIndexPerShader.count(drawMeshCommand->GetMaterial()->GetShaderName()) > 0 ? constantBufferIndexPerShader[drawMeshCommand->GetMaterial()->GetShaderName()]: 0;

            if (drawMeshCommand->GetMesh() == nullptr || drawMeshCommand->GetMesh()->GetVertices().size() == 1) continue; // It's a light

            cam = drawMeshCommand->GetCamera() != nullptr ? drawMeshCommand->GetCamera().get(): Camera::main;

            CameraPass cameraPass = {
                cam->GetViewMatrix(),
                cam->GetViewMatrix().GetInverse(),
                cam->GetProjection(),
                cam->GetProjection().GetInverse(),
                cam->GetViewProjection(),
                cam->GetViewProjection().GetInverse(),
                cam->GetPosition(),
                0,
                cam->GetDisplaySize(),
                { 1 / cam->GetDisplaySize().GetX(), 1 / cam->GetDisplaySize().GetY() },
                cam->GetNearZ(),
                cam->GetFarZ(),
                Time::GetTotalTime(),
                Time::GetDeltaTime()
            };
            
            Ref<MetalShader> metalShader = std::dynamic_pointer_cast<MetalShader>(Shader::GetShader(drawMeshCommand->GetMaterial()->GetShaderName())->GetInternalShader());

            drawMeshCommand->GetMesh()->BuildVertexBuffer(metalShader->GetAttributes());
            if (itemsCount >= bufferGroups.size()) {
                bufferGroups.push_back(CreateRef<MetalBufferGroup>(drawMeshCommand->GetMesh()));
                objectTransforms.push_back(drawMeshCommand->GetTransform());
            } else {
                bufferGroups[itemsCount]->SetData(drawMeshCommand->GetMesh());
                objectTransforms[itemsCount] = drawMeshCommand->GetTransform();
            }

            void* material1 = drawMeshCommand->GetMaterial()->GetDataPointer();
           
            metalShader->UseShader((rtvEncoder != nullptr ? rtvEncoder : encoder));
            metalShader->Bind();
            metalShader->SetObjectIndex(shapes);
            if (bufferGroups[itemsCount]->GetVertexBuffer() != nullptr)
                metalShader->SetVertexBufferIndex(bufferGroups[itemsCount]->GetVertexBuffer());
            metalShader->SetData(&cameraPass, Shader::CameraBufferName, sizeof(CameraPass), shapes);
            metalShader->SetData(&objectTransforms[itemsCount], Shader::ObjectBufferName, sizeof(Matrix4x4), shapes);
            metalShader->SetData(material1, materialName.c_str(), drawMeshCommand->GetMaterial()->GetSize(), shapes);
            metalShader->SetData(lights, Light::LightName, sizeof(Light) * Light::LightsMax, shapes);
            metalShader->SetData(spacesPointer, Light::LightSpaceName, sizeof(Matrix4x4) * Light::LightsMax, shapes);
            metalShader->SetTextures(shadowMap, drawMeshCommand->GetMaterial());
            
            metalShader->Draw(GetTopology(drawMeshCommand->GetMesh()->GetSubMesh(drawMeshCommand->GetSubmeshIndex()).GetTopology()), bufferGroups[itemsCount]->GetIndexBuffer());
            free(material1);
            itemsCount++;
            constantBufferIndexPerShader[drawMeshCommand->GetMaterial()->GetShaderName()] = ++shapes;
        }
    }

    void MetalGraphics::DrawPostProcess() {
        
    }
}

#endif
