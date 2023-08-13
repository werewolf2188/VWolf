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
        if (renderTexture != nullptr) {
            ((OpenGLRenderTexture*)renderTexture.get())->Bind();
        }
    }

    void OpenGLGraphics::UnbindToRenderTexture(){
        if (renderTexture != nullptr) {
            ((OpenGLRenderTexture*)renderTexture.get())->Unbind();
        }
    }

    void OpenGLGraphics::ClearColorImpl(Color color) {
        BindToRenderTexture();
        GLThrowIfFailed(glClearColor(color.r, color.b, color.g, color.a));
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

    void OpenGLGraphics::DrawMeshImpl(MeshData& mesh, Vector4Float position, Vector4Float rotation, Material& material, Ref<Camera> camera) {
        auto data = mesh.vertices;
        auto indices = mesh.indices;
        Ref<OpenGLVertexBuffer> vertices = CreateRef<OpenGLVertexBuffer>(data.data(), data.size() * sizeof(Vertex));
        vertices->SetLayout(MeshData::Layout);
        Ref<OpenGLIndexBuffer> index = CreateRef<OpenGLIndexBuffer>(indices.data(), indices.size());
        Ref<OpenGLVertexArray> group = CreateRef<OpenGLVertexArray>(vertices);
        
        Camera* cam = camera != nullptr ? camera.get(): Camera::main;

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
        
        MatrixFloat4x4 transform = translate(MatrixFloat4x4(1.0f), Vector3Float(position));
        transform = VWolf::rotate(transform, rotation.x, { 1.0f, 0.0f, 0.0f });
        transform = VWolf::rotate(transform, rotation.y, { 0.0f, 1.0f, 0.0f });
        transform = VWolf::rotate(transform, rotation.z, { 0.0f, 0.0f, 1.0f });

        Ref<Shader> shader = ShaderLibrary::GetShader(material.GetName().c_str());
        void* material1 = material.GetDataPointer();
        Light* lights = this->lights.data();
        shader->Bind();
        shader->SetData(&cameraPass, ShaderLibrary::CameraBufferName, sizeof(CameraPass), 0);
        shader->SetData(&transform, ShaderLibrary::ObjectBufferName, sizeof(MatrixFloat4x4), 0);
        shader->SetData(material1, materialName.c_str(), material.GetSize(), 0);
        shader->SetData(lights, Light::LightName, sizeof(Light) * Light::LightsMax, 0);
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
        shader->Unbind();
        free(material1);
    }

    void OpenGLGraphics::RenderMeshImpl(MeshData& mesh, MatrixFloat4x4 transform, Material& material, Ref<Camera> camera) {
        auto data = mesh.vertices;
        auto indices = mesh.indices;
        Ref<OpenGLVertexBuffer> vertices = CreateRef<OpenGLVertexBuffer>(data.data(), data.size() * sizeof(Vertex));
        vertices->SetLayout(MeshData::Layout);
        Ref<OpenGLIndexBuffer> index = CreateRef<OpenGLIndexBuffer>(indices.data(), indices.size());
        Ref<OpenGLVertexArray> group = CreateRef<OpenGLVertexArray>(vertices);
        
        Camera* cam = camera != nullptr ? camera.get(): Camera::main;

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

        Ref<Shader> shader = ShaderLibrary::GetShader(material.GetName().c_str());
        void* material1 = material.GetDataPointer();
        Light* lights = this->lights.data();
        std::vector<ShaderInput> textures = shader->GetTextureInputs();
        shader->Bind();
        for (GLuint index = 0; index < textures.size(); index++) {
            OpenGLBindableTexture* texture = dynamic_cast<OpenGLBindableTexture*>(material.GetTexture(textures[index].GetName()).get());
            if (texture != nullptr) {
                texture->Bind(index);
                // TODO: Move this inside bind
                GLThrowIfFailed(glUniform1i(textures[index].GetIndex(), index));
            }
//            OpenGLCubemap* cubeMap = dynamic_cast<OpenGLCubemap*>(material.GetTexture(textures[index].GetName()).get());
//            if (cubeMap != nullptr) {
//                cubeMap->Bind(index);
//                GLThrowIfFailed(glUniform1i(textures[index].GetIndex(), index));
//            }
        }
        shader->SetData(&cameraPass, ShaderLibrary::CameraBufferName, sizeof(CameraPass), 0);
        shader->SetData(&transform, ShaderLibrary::ObjectBufferName, sizeof(MatrixFloat4x4), 0);
        shader->SetData(material1, materialName.c_str(), material.GetSize(), 0);
        if (lights)
            shader->SetData(lights, Light::LightName, sizeof(Light) * Light::LightsMax, 0);
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
            OpenGLTexture2D* texture = dynamic_cast<OpenGLTexture2D*>(material.GetTexture(textures[index].GetName()).get());
            if (texture != nullptr) {
                texture->Unbind(index);
            }
            OpenGLCubemap* cubeMap = dynamic_cast<OpenGLCubemap*>(material.GetTexture(textures[index].GetName()).get());
            if (cubeMap != nullptr) {
                cubeMap->Unbind(index);
            }
        }
        shader->Unbind();
        
        free(material1);
    }

    void OpenGLGraphics::BeginFrameImpl()
    {
    }
    void OpenGLGraphics::EndFrameImpl()
    {
    }

    void OpenGLGraphics::SetRenderTextureImpl(Ref<RenderTexture> renderTexture)
    {
        this->renderTexture = renderTexture;
    }
}
