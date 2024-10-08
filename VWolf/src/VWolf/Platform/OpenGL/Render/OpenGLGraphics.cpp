//
//  OpenGLRenderer.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 10/11/22.
//

#include "vwpch.h"
#include "OpenGLGraphics.h"

#include "VWolf/Core/Render/RenderItem.h"


#include "VWolf/Platform/OpenGL/Core/GLCore.h"

namespace VWolf {
    void OpenGLGraphics::Initialize() {
        shadowMap = CreateRef<OpenGLRenderTexture>(1024, 1024, true, TextureOptions());
        emptyShadowMap = CreateRef<OpenGLTexture2D>(TextureDefault::White, 1024, 1024, TextureOptions());
    }
// TODO: For the future on how to create render queue
//    void OpenGLRenderer::ProcessItems() {
//        
//        std::vector<Ref<OpenGLVertexArray>> groups;
//        for (auto renderItem: items) {
//            auto data = renderItem->data.GetVertices();
//            auto indices = renderItem->data.indices;
//            Ref<OpenGLVertexBuffer> vertices = CreateRef<OpenGLVertexBuffer>(m_window, data.data(), data.size() * sizeof(float));
//            vertices->SetLayout(MeshData::Layout);
//            Ref<OpenGLIndexBuffer> index = CreateRef<OpenGLIndexBuffer>(m_window, indices.data(), indices.size());
//            Ref<OpenGLVertexArray> group = CreateRef<OpenGLVertexArray>(m_window);
//            group->AddVertexBuffer(vertices);
//            group->SetIndexBuffer(index);
//            groups.push_back(group);
//        }
//        
//        CameraPass projection = GetCameraPass();
//        
//        
//        if (clearColor)
//            glClearColor(backgroundColor.r, backgroundColor.b, backgroundColor.g, backgroundColor.a);
//        if (clearDepthStencil)
//            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//        for (int i = 0; i < items.size(); i++) {
//            Ref<Shader> shader = ShaderLibrary::GetShader(items[i]->material.GetShader().c_str());
//            void* material = items[i]->material.GetDataPointer();
//            Light* lights = this->lights.data();
//            shader->Bind();
//            shader->SetData(&projection, ShaderLibrary::CameraBufferName, sizeof(CameraPass), 0);
//            shader->SetData(&items[i]->transform, ShaderLibrary::ObjectBufferName, sizeof(MatrixFloat4x4), 0);
//            shader->SetData(material, items[i]->material.GetName(), items[i]->material.GetSize(), 0);
//            shader->SetData(lights, Light::LightName, sizeof(Light) * Light::LightsMax, 0);
//            groups[i]->Bind();
//            uint32_t count = groups[i]->GetIndexBuffer()->GetCount();
//            glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
//            groups[i]->Unbind();
//            shader->Unbind();
//        }
//
//        clearColor = false;
//        clearDepthStencil = false;
//    }

    void OpenGLGraphics::BindToRenderTexture() {
        if (renderTexture != nullptr && useRenderTexture) {
            ((OpenGLRenderTexture*)renderTexture.get())->Bind();
        }
    }

    void OpenGLGraphics::UnbindToRenderTexture(){
        if (renderTexture != nullptr && useRenderTexture) {
            ((OpenGLRenderTexture*)renderTexture.get())->Unbind();
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
        lights.clear();
    }

    void OpenGLGraphics::AddLightImpl(Light& light) {
        lights.push_back(light);
    }

    // TODO: Better names. This is for immediate rendering
    void OpenGLGraphics::DrawMeshImpl(MeshData& mesh, Vector4 position, Vector4 rotation, Material& material, Ref<Camera> camera) {
        auto data = mesh.vertices;
        auto indices = mesh.indices;
        Ref<OpenGLVertexBuffer> vertices = CreateRef<OpenGLVertexBuffer>(data.data(), data.size() * sizeof(Vertex));
        vertices->SetLayout(MeshData::Layout);
        Ref<OpenGLIndexBuffer> index = CreateRef<OpenGLIndexBuffer>(indices.data(), indices.size());
        Ref<OpenGLVertexArray> group = CreateRef<OpenGLVertexArray>(vertices);
        
        Camera* cam = camera != nullptr ? camera.get(): Camera::main;

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

        Matrix4x4 transform = Matrix4x4::TRS(Vector3(position),
                                             Quaternion::Euler(rotation.GetX(), rotation.GetY(), rotation.GetZ()),
                                             Vector3::One);

        Ref<Shader> shader = ShaderLibrary::GetShader(material.GetShaderName().c_str());
        void* material1 = material.GetDataPointer();
        Light* lights = this->lights.data();
        std::vector<ShaderInput> textures = shader->GetTextureInputs();
        shader->Bind();
        for (GLuint index = 0; index < textures.size(); index++) {
            if (textures[index].GetName() == "u_shadow") {
                emptyShadowMap->Bind(index);
            } else {
                OpenGLBindableTexture* texture = dynamic_cast<OpenGLBindableTexture*>(material.GetTexture(textures[index].GetName()).get());
                if (texture != nullptr) {
                    texture->Bind(index);
                    // TODO: Move this inside bind
                    GLThrowIfFailed(glUniform1i(textures[index].GetIndex(), index));
                }
            }
        }
        shader->Bind();
        shader->SetData(&cameraPass, ShaderLibrary::CameraBufferName, sizeof(CameraPass), 0);
        shader->SetData(&transform, ShaderLibrary::ObjectBufferName, sizeof(Matrix4x4), 0);
        shader->SetData(material1, materialName.c_str(), material.GetSize(), 0);
        if (lights) {
            shader->SetData(lights, Light::LightName, sizeof(Light) * Light::LightsMax, 0);
            std::vector<Matrix4x4> spaces;
            for(int i = 0; i < this->lights.size(); i++) {
                spaces.push_back(lights[i].GetLightSpaceMatrix());
            }
            Matrix4x4* spacesPointer = spaces.data();
            shader->SetData(spacesPointer, Light::LightSpaceName, sizeof(Matrix4x4) * Light::LightsMax, 0);
        }
        group->Bind();
        vertices->Bind();
        index->Bind();
        uint32_t count = index->GetCount();
        BindToRenderTexture();
        GLThrowIfFailed(glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr));
        UnbindToRenderTexture();
        vertices->Unbind();
        index->Unbind();
        group->Unbind();
        for (GLuint index = 0; index < textures.size(); index++) {
            if (textures[index].GetName() == "u_shadow") {
                emptyShadowMap->Unbind(index);
            } else {
                OpenGLBindableTexture* texture = dynamic_cast<OpenGLBindableTexture*>(material.GetTexture(textures[index].GetName()).get());
                if (texture != nullptr) {
                    texture->Unbind(index);
                }
            }
        }
        shader->Unbind();
        free(material1);
    }

    // TODO: Better names. This is for lazy rendering
    void OpenGLGraphics::RenderMeshImpl(MeshData& mesh, Matrix4x4 transform, Material& material, Ref<Camera> camera) {
        items.push_back(CreateRef<RenderItem>(mesh, material, transform, camera));
    }

    void OpenGLGraphics::BeginFrameImpl()
    {
    }
    void OpenGLGraphics::EndFrameImpl()
    {
        
    }

    void OpenGLGraphics::BeginSceneImpl() {
        useRenderTexture = true;
        items.clear();
    }

    void OpenGLGraphics::EndSceneImpl() {
        DrawShadowMap();
        DrawQueue();
        DrawPostProcess();
        useRenderTexture = false;
    }

    void OpenGLGraphics::SetRenderTextureImpl(Ref<RenderTexture> renderTexture)
    {
        this->renderTexture = renderTexture;
    }

    void OpenGLGraphics::DrawShadowMap() {
        shadowMap->Bind();
        GLThrowIfFailed(glClear(GL_DEPTH_BUFFER_BIT));
        for (Light& light: lights) {
            Ref<Shader> shader = ShaderLibrary::GetShader("Shadow"); // TODO: Organize this
            for(Ref<RenderItem> item: items) {
                auto& mesh = item->data;

                auto data = mesh.vertices;
                if (data.size() == 1) continue;; // It's a light
                auto indices = mesh.indices;
                Ref<OpenGLVertexBuffer> vertices = CreateRef<OpenGLVertexBuffer>(data.data(), data.size() * sizeof(Vertex));
                vertices->SetLayout(MeshData::Layout);
                Ref<OpenGLIndexBuffer> index = CreateRef<OpenGLIndexBuffer>(indices.data(), indices.size());
                Ref<OpenGLVertexArray> group = CreateRef<OpenGLVertexArray>(vertices);

                Matrix4x4 viewProjection = light.GetLightSpaceMatrix();
                Matrix4x4 transform = item->transform;
                
                shader->Bind();
                shader->SetData(&viewProjection, ShaderLibrary::CameraBufferName, sizeof(Matrix4x4), 0);
                shader->SetData(&transform, ShaderLibrary::ObjectBufferName, sizeof(Matrix4x4), 0);

                group->Bind();
                vertices->Bind();
                index->Bind();
                
                uint32_t count = index->GetCount();
                GLThrowIfFailed(glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr));

                vertices->Unbind();
                index->Unbind();
                group->Unbind();
                shader->Unbind();
            }
        }
        shadowMap->Unbind();
    }

    void OpenGLGraphics::DrawQueue() {
        for(Ref<RenderItem> item: items) {
            auto& mesh = item->data;
            auto& material = item->material;
            Ref<Camera> camera = item->camera;
            Matrix4x4 transform = item->transform;

            auto data = mesh.vertices;
            auto indices = mesh.indices;
            Ref<OpenGLVertexBuffer> vertices = CreateRef<OpenGLVertexBuffer>(data.data(), data.size() * sizeof(Vertex));
            vertices->SetLayout(MeshData::Layout);
            Ref<OpenGLIndexBuffer> index = CreateRef<OpenGLIndexBuffer>(indices.data(), indices.size());
            Ref<OpenGLVertexArray> group = CreateRef<OpenGLVertexArray>(vertices);
            
            Camera* cam = camera != nullptr ? camera.get(): Camera::main;

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

            Ref<Shader> shader = ShaderLibrary::GetShader(material.GetShaderName().c_str());
            void* material1 = material.GetDataPointer();
            Light* lights = this->lights.data();
            std::vector<ShaderInput> textures = shader->GetTextureInputs();
            shader->Bind();
            for (GLuint index = 0; index < textures.size(); index++) {
                if (textures[index].GetName() == "u_shadow") {
                    shadowMap->DepthTextureBind(index);
                    GLThrowIfFailed(glUniform1i(textures[index].GetIndex(), index));
                } else {
                    OpenGLBindableTexture* texture = dynamic_cast<OpenGLBindableTexture*>(material.GetTexture(textures[index].GetName()).get());
                    if (texture != nullptr) {
                        texture->Bind(index);
                        // TODO: Move this inside bind
                        GLThrowIfFailed(glUniform1i(textures[index].GetIndex(), index));
                    }
                }
            }
            shader->SetData(&cameraPass, ShaderLibrary::CameraBufferName, sizeof(CameraPass), 0);
            shader->SetData(&transform, ShaderLibrary::ObjectBufferName, sizeof(Matrix4x4), 0);
            shader->SetData(material1, materialName.c_str(), material.GetSize(), 0);
            if (lights) {
                shader->SetData(lights, Light::LightName, sizeof(Light) * Light::LightsMax, 0);
                std::vector<Matrix4x4> spaces;
                for(int i = 0; i < this->lights.size(); i++) {
                    spaces.push_back(lights[i].GetLightSpaceMatrix());
                }
                Matrix4x4* spacesPointer = spaces.data();
                shader->SetData(spacesPointer, Light::LightSpaceName, sizeof(Matrix4x4) * Light::LightsMax, 0);
            }
            
            group->Bind();
            vertices->Bind();
            index->Bind();
            
            uint32_t count = index->GetCount();
            BindToRenderTexture();
            GLThrowIfFailed(glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr));
            UnbindToRenderTexture();
            vertices->Unbind();
            index->Unbind();
            group->Unbind();
            for (GLuint index = 0; index < textures.size(); index++) {
                if (textures[index].GetName() == "u_shadow") {
                    shadowMap->DepthTextureUnbind(index);
                } else {
                    OpenGLBindableTexture* texture = dynamic_cast<OpenGLBindableTexture*>(material.GetTexture(textures[index].GetName()).get());
                    if (texture != nullptr) {
                        texture->Unbind(index);
                    }
                }
            }
            shader->Unbind();
            
            free(material1);
        }
    }

    void OpenGLGraphics::DrawPostProcess() {
        
    }
}
