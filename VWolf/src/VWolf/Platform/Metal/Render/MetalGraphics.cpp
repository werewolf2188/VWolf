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

    void MetalGraphics::Initialize() {
        emptyShadowMap = CreateRef<MetalTexture2D>(TextureDefault::White, 1024, 1024, TextureOptions());
        shadowMap = CreateRef<MetalRenderTexture>(1024, 1024, true);
    }

    void MetalGraphics::DrawMeshImpl(MeshData& mesh, Vector4 position, Vector4 rotation, Material& material, Ref<Camera> camera) {
        MTL::RenderCommandEncoder* rtvEncoder = nullptr;

        int shapes = constantBufferIndexPerShader.count(material.GetShaderName()) > 0 ? constantBufferIndexPerShader[material.GetShaderName()]: 0;
        if (renderTexture) {
            rtvEncoder = ((MetalRenderTexture*)renderTexture.get())->StartEncoder();
        }

        Camera* cam = nullptr;

        Matrix4x4 transform = Matrix4x4::TRS(Vector3(position),
                                             Quaternion::Euler(rotation.GetX(), rotation.GetY(), rotation.GetZ()),
                                             Vector3::One);

        if (mesh.vertices.size() == 1) return;; // It's a light

        cam = camera != nullptr ? camera.get(): Camera::main;

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

        if (itemsCount >= bufferGroups.size()) {
            bufferGroups.push_back(CreateRef<MetalBufferGroup>(mesh));
            objectTransforms.push_back(transform);
        } else {
            bufferGroups[itemsCount]->SetData(mesh);
            objectTransforms[itemsCount] = transform;
        }

        void* material1 = material.GetDataPointer();
        Ref<Shader> shader = ShaderLibrary::GetShader(material.GetShaderName());
        ((MSLShader*)shader.get())->UseShader((rtvEncoder != nullptr ? rtvEncoder : encoder));
        shader->Bind();
        (rtvEncoder != nullptr ? rtvEncoder : encoder)->setVertexBuffer(*bufferGroups[itemsCount]->GetVertexBuffer(), 0, ((MSLShader*)shader.get())->GetVertexBufferIndex());
        shader->SetData(&cameraPass, ShaderLibrary::CameraBufferName, sizeof(CameraPass), shapes);
        shader->SetData(&objectTransforms[itemsCount], ShaderLibrary::ObjectBufferName, sizeof(Matrix4x4), shapes);
        shader->SetData(material1, materialName.c_str(), material.GetSize(), shapes);

        for (auto textureInput : shader->GetTextureInputs()) {
            if (textureInput.GetName() == "Shadow") {
                (rtvEncoder != nullptr ? rtvEncoder : encoder)->setFragmentTexture(reinterpret_cast<MTL::Texture*>(emptyShadowMap->GetHandler()), textureInput.GetIndex());
            } else {
                Ref<Texture> texture = material.GetTexture(textureInput.GetName());
                if (texture != nullptr) {
                    (rtvEncoder != nullptr ? rtvEncoder : encoder)->setFragmentTexture(reinterpret_cast<MTL::Texture*>(texture->GetHandler()), textureInput.GetIndex());
                }
            }
            
        }
        
        (rtvEncoder != nullptr ? rtvEncoder : encoder)->drawIndexedPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, bufferGroups[itemsCount]->GetIndexBuffer()->GetCount(), bufferGroups[itemsCount]->GetIndexBuffer()->GetType(), *bufferGroups[itemsCount]->GetIndexBuffer(), 0);
        free(material1);

        itemsCount++;
        constantBufferIndexPerShader[material.GetShaderName()] = ++shapes;
    }

    void MetalGraphics::RenderMeshImpl(MeshData& mesh, Matrix4x4 transform, Material& material, Ref<Camera> camera) {
        items.push_back(CreateRef<RenderItem>(mesh, material, transform, camera));
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

    void MetalGraphics::AddLightImpl(Light& light) {
        lights.push_back(light);
        spaces.push_back(light.GetLightSpaceMatrix());
    }

    void MetalGraphics::BeginFrameImpl() {
        pool = NS::AutoreleasePool::alloc()->init();
        commandBuffer =  MetalDriver::GetCurrent()->GetCommand()->GetCommandQueue()->commandBuffer();
        if (renderTexture) {
            ((MetalRenderTexture*)renderTexture.get())->Prepare();
        }
        shadowMap->Prepare();
        MetalDriver::GetCurrent()->GetSurface()->Begin();

        MetalDriver::GetCurrent()->GetSurface()->GetRenderPassDescriptor()->colorAttachments()->object(0)->setTexture(MetalDriver::GetCurrent()->GetSurface()->GetCurrentDrawable()->texture());

        lights.clear();
        spaces.clear();
        constantBufferIndexPerShader.clear();
        itemsCount = 0;
        if (renderTexture) {
            ((MetalRenderTexture*)renderTexture.get())->StartEncoder();
        }
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
        this->renderTexture = renderTexture;
        
    }

    void MetalGraphics::BeginSceneImpl() {
        items.clear();
    }

    void MetalGraphics::EndSceneImpl() {
        encoder = commandBuffer->renderCommandEncoder(MetalDriver::GetCurrent()->GetSurface()->GetRenderPassDescriptor());
        DrawShadowMap();
        DrawQueue();
        DrawPostProcess();
        
    }

    void MetalGraphics::DrawShadowMap() {
        MTL::RenderCommandEncoder* dsvEncoder = shadowMap->StartEncoder();

        for (Light& light : lights) {
            int shadowShapes = 0;
            Matrix4x4 viewProjection = light.GetLightSpaceMatrix();//cam->GetViewProjection();
            for (auto item: items) {
                if (item->data.vertices.size() == 1) continue; // It's a light
    
                if (shadowShapes >= shadowBufferGroups.size()) {
                    shadowBufferGroups.push_back(CreateRef<MetalBufferGroup>(item->data));
                    shadowObjectTransforms.push_back(item->transform);
                } else {
                    shadowBufferGroups[shadowShapes]->SetData(item->data);
                    shadowObjectTransforms[shadowShapes] = item->transform;
                }
                Ref<Shader> shader = ShaderLibrary::GetShader("Shadow");
                
                ((MSLShader*)shader.get())->UseShader(dsvEncoder);
                shader->Bind();
                dsvEncoder->setVertexBuffer(*shadowBufferGroups[shadowShapes]->GetVertexBuffer(), 0, ((MSLShader*)shader.get())->GetVertexBufferIndex());
                shader->SetData(&viewProjection, ShaderLibrary::CameraBufferName, sizeof(Matrix4x4), shadowShapes);
                shader->SetData(&shadowObjectTransforms[shadowShapes], ShaderLibrary::ObjectBufferName, sizeof(Matrix4x4), shadowShapes);

                dsvEncoder->drawIndexedPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, shadowBufferGroups[shadowShapes]->GetIndexBuffer()->GetCount(), shadowBufferGroups[shadowShapes]->GetIndexBuffer()->GetType(), *shadowBufferGroups[shadowShapes]->GetIndexBuffer(), 0);
                shadowShapes++;
            }
        }

        shadowMap->Commit();
    }

    void MetalGraphics::DrawQueue() {
        
        MTL::RenderCommandEncoder* rtvEncoder = nullptr;
        if (renderTexture) {
            rtvEncoder = ((MetalRenderTexture*)renderTexture.get())->StartEncoder();
        }

        if (this->lights.size() == 0) {
            this->lights.push_back(Light());
        }
        if (this->spaces.size() == 0) {
            this->spaces.push_back(Matrix4x4());
        }
        Light* lights = this->lights.data();
        Matrix4x4* spacesPointer = spaces.data();

        Camera* cam = nullptr;

        for (auto item: items) {
    
            int shapes = constantBufferIndexPerShader.count(item->material.GetShaderName()) > 0 ? constantBufferIndexPerShader[item->material.GetShaderName()]: 0;

            if (item->data.vertices.size() == 1) continue;; // It's a light

            cam = item->camera != nullptr ? item->camera.get(): Camera::main;

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

            if (itemsCount >= bufferGroups.size()) {
                bufferGroups.push_back(CreateRef<MetalBufferGroup>(item->data));
                objectTransforms.push_back(item->transform);
            } else {
                bufferGroups[itemsCount]->SetData(item->data);
                objectTransforms[itemsCount] = item->transform;
            }

            void* material1 = item->material.GetDataPointer();
            Ref<Shader> shader = ShaderLibrary::GetShader(item->material.GetShaderName());
            ((MSLShader*)shader.get())->UseShader((rtvEncoder != nullptr ? rtvEncoder : encoder));
            shader->Bind();
            (rtvEncoder != nullptr ? rtvEncoder : encoder)->setVertexBuffer(*bufferGroups[itemsCount]->GetVertexBuffer(), 0, ((MSLShader*)shader.get())->GetVertexBufferIndex());
            shader->SetData(&cameraPass, ShaderLibrary::CameraBufferName, sizeof(CameraPass), shapes);
            shader->SetData(&objectTransforms[itemsCount], ShaderLibrary::ObjectBufferName, sizeof(Matrix4x4), shapes);
            shader->SetData(material1, materialName.c_str(), item->material.GetSize(), shapes);
            shader->SetData(lights, Light::LightName, sizeof(Light) * Light::LightsMax, shapes);
            shader->SetData(spacesPointer, Light::LightSpaceName, sizeof(Matrix4x4) * Light::LightsMax, shapes);

            for (auto textureInput : shader->GetTextureInputs()) {
                if (textureInput.GetName() == "Shadow") {
                    (rtvEncoder != nullptr ? rtvEncoder : encoder)->setFragmentTexture(reinterpret_cast<MTL::Texture*>(shadowMap->GetHandler()), textureInput.GetIndex());
                } else {
                    Ref<Texture> texture = item->material.GetTexture(textureInput.GetName());
                    if (texture != nullptr) {
                        (rtvEncoder != nullptr ? rtvEncoder : encoder)->setFragmentTexture(reinterpret_cast<MTL::Texture*>(texture->GetHandler()), textureInput.GetIndex());
                    }
                }
                
            }
            
            (rtvEncoder != nullptr ? rtvEncoder : encoder)->drawIndexedPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, bufferGroups[itemsCount]->GetIndexBuffer()->GetCount(), bufferGroups[itemsCount]->GetIndexBuffer()->GetType(), *bufferGroups[itemsCount]->GetIndexBuffer(), 0);
            free(material1);
    
            itemsCount++;
            constantBufferIndexPerShader[item->material.GetShaderName()] = ++shapes;
        }
    }

    void MetalGraphics::DrawPostProcess() {
        
    }
}

#endif
