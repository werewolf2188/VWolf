//
//  MetalGraphics.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 4/19/24.
//

#include "vwpch.h"

#if defined(VWOLF_PLATFORM_MACOS) || defined(VWOLF_PLATFORM_IOS)
#include "MetalGraphics.h"

#include "MSLShader.h"

#include "VWolf/Core/Render/RenderItem.h"
#include "VWolf/Platform/Metal/MetalDriver.h"
#include "VWolf/Platform/Metal/Render/MetalTexture.h"

#include "VWolf/Core/Debug/ShapeHelper.h"

namespace VWolf {
    MeshData box = ShapeHelper::CreateBox(10, 10, 10, 0);

    void MetalGraphics::Initialize() {}

    void MetalGraphics::DrawMeshImpl(MeshData& mesh, Vector4Float position, Vector4Float rotation, Material& material, Ref<Camera> camera) {
        
    }

    void MetalGraphics::RenderMeshImpl(MeshData& mesh, MatrixFloat4x4 transform, Material& material, Ref<Camera> camera) {
        items.push_back(CreateRef<RenderItem>(mesh, material, transform, camera));
    }

    void MetalGraphics::ClearColorImpl(Color color) {
        MetalDriver::GetCurrent()->GetSurface()->GetRenderPassDescriptor()->colorAttachments()->object(0)->setClearColor(MTL::ClearColor::Make(color.r, color.g, color.b, color.a));
        if (renderTexture) {
            auto metalRenderTexture = (MetalRenderTexture*)renderTexture.get();
            metalRenderTexture->GetRenderPassDescriptor()->colorAttachments()->object(0)->setClearColor(MTL::ClearColor::Make(color.r, color.g, color.b, color.a));
        }
    }

    void MetalGraphics::ClearImpl() {
        MetalDriver::GetCurrent()->GetSurface()->GetRenderPassDescriptor()->colorAttachments()->object(0)->setLoadAction(MTL::LoadAction::LoadActionClear);
        if (renderTexture) {
            auto metalRenderTexture = (MetalRenderTexture*)renderTexture.get();
            metalRenderTexture->GetRenderPassDescriptor()->colorAttachments()->object(0)->setLoadAction(MTL::LoadAction::LoadActionClear);
        }
    }

    void MetalGraphics::AddLightImpl(Light& light) {
        
    }

    void MetalGraphics::BeginFrameImpl() {
        pool = NS::AutoreleasePool::alloc()->init();
        commandBuffer =  MetalDriver::GetCurrent()->GetCommand()->GetCommandQueue()->commandBuffer();
        if (renderTexture) {
            ((MetalRenderTexture*)renderTexture.get())->Prepare();
        }
        MetalDriver::GetCurrent()->GetSurface()->Begin();

        MetalDriver::GetCurrent()->GetSurface()->GetRenderPassDescriptor()->colorAttachments()->object(0)->setTexture(MetalDriver::GetCurrent()->GetSurface()->GetCurrentDrawable()->texture());
    }

    void MetalGraphics::EndFrameImpl() {
        
        encoder->endEncoding();
        commandBuffer->presentDrawable(MetalDriver::GetCurrent()->GetSurface()->GetCurrentDrawable());
        commandBuffer->commit();
        
        pool->release();
        pool = nullptr;
    }

    void MetalGraphics::SetRenderTextureImpl(Ref<RenderTexture> renderTexture) {
        this->renderTexture = renderTexture;
        
    }

    void MetalGraphics::BeginSceneImpl() {
        items.clear();
    }

    void MetalGraphics::EndSceneImpl() {
        encoder = commandBuffer->renderCommandEncoder(MetalDriver::GetCurrent()->GetSurface()->GetRenderPassDescriptor());
        MTL::RenderCommandEncoder* rtvEncoder = nullptr;
        if (renderTexture) {
            rtvEncoder = ((MetalRenderTexture*)renderTexture.get())->StartEncoder();
        }

        Camera* cam = Camera::main;

        CameraPass cameraPass = {
            cam->GetViewMatrix(),
            inverse(cam->GetViewMatrix()),
            cam->GetProjection(),
            inverse(cam->GetProjection()),
            cam->GetViewProjection(),
            inverse(cam->GetViewProjection()),
            cam->GetPosition(),
            0,
            cam->GetDisplaySize(),
            { 1 / cam->GetDisplaySize().x, 1 / cam->GetDisplaySize().y },
            cam->GetNearZ(),
            cam->GetFarZ(),
            Time::GetTotalTime(),
            Time::GetDeltaTime()
        };

        for (int index = 0; index < items.size(); index++) {
            if (index >= bufferGroups.size()) {
                bufferGroups.push_back(CreateRef<MetalBufferGroup>(items[index]->data));
                objectTransforms.push_back(items[index]->transform);
            } else {
                bufferGroups[index]->SetData(items[index]->data);
                objectTransforms[index] = items[index]->transform;
            }
        }

        for (int index = 0; index < items.size(); index++) {
            Ref<Shader> shader = ShaderLibrary::GetShader(items[index]->material.GetShaderName());
            ((MSLShader*)shader.get())->UseShader((rtvEncoder != nullptr ? rtvEncoder : encoder));
            shader->Bind();
            (rtvEncoder != nullptr ? rtvEncoder : encoder)->setVertexBuffer(*bufferGroups[index]->GetVertexBuffer(), 0, ((MSLShader*)shader.get())->GetVertexBufferIndex());
            shader->SetData(&cameraPass, ShaderLibrary::CameraBufferName, sizeof(CameraPass), index);
            shader->SetData(&objectTransforms[index], ShaderLibrary::ObjectBufferName, sizeof(MatrixFloat4x4), index);

            for (auto textureInput : shader->GetTextureInputs()) {
                Ref<Texture> texture = items[index]->material.GetTexture(textureInput.GetName());
                if (texture != nullptr) {
                    (rtvEncoder != nullptr ? rtvEncoder : encoder)->setFragmentTexture(reinterpret_cast<MTL::Texture*>(texture->GetHandler()), textureInput.GetIndex());
                }
            }
            
            (rtvEncoder != nullptr ? rtvEncoder : encoder)->drawIndexedPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, bufferGroups[index]->GetIndexBuffer()->GetCount(), bufferGroups[index]->GetIndexBuffer()->GetType(), *bufferGroups[index]->GetIndexBuffer(), 0);
        }
        if (rtvEncoder != nullptr) {
            ((MetalRenderTexture*)renderTexture.get())->Commit();
        }
    }

    void MetalGraphics::DrawShadowMap() {
        
    }

    void MetalGraphics::DrawQueue() {
        
    }

    void MetalGraphics::DrawPostProcess() {
        
    }
}

#endif
