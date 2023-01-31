//
//  OpenGLRenderer.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 10/11/22.
//

#include "vwpch.h"
#include "OpenGLRenderer.h"

#include "VWolf/Core/Render/RenderItem.h"

namespace VWolf {
    void OpenGLGraphics::Build() {
#ifdef VWOLF_PLATFORM_WINDOWS
        const char *vertexShaderText = "src/shaders/glsl/Grid.vert.glsl";
        const char *fragmentShaderText = "src/shaders/glsl/Grid.frag.glsl";
#else
        const char *vertexShaderText = "../../../Sandbox/src/shaders/glsl/Grid.vert.glsl";
        const char *fragmentShaderText = "../../../Sandbox/src/shaders/glsl/Grid.frag.glsl";
#endif

        ShaderSource vertexSource = { ShaderType::Vertex, ShaderSourceType::File, vertexShaderText };
        ShaderSource fragmentSource = { ShaderType::Fragment, ShaderSourceType::File, fragmentShaderText };

        std::initializer_list<ShaderSource> otherShaders = { vertexSource, fragmentSource };
        gridShader = CreateRef<GLSLShader>("Grid", MeshData::Layout, otherShaders);
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
        glClearColor(color.r, color.b, color.g, color.a);
        UnbindToRenderTexture();
    }

    void OpenGLGraphics::ClearImpl() {
        BindToRenderTexture();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
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
        glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
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
            OpenGLTexture2D* texture = dynamic_cast<OpenGLTexture2D*>(material.GetTexture(textures[index].GetName()).get());
            if (texture != nullptr) {
                texture->Bind(index);
                glUniform1i(textures[index].GetIndex(), index);
            }
        }
        shader->SetData(&cameraPass, ShaderLibrary::CameraBufferName, sizeof(CameraPass), 0);
        shader->SetData(&transform, ShaderLibrary::ObjectBufferName, sizeof(MatrixFloat4x4), 0);
        shader->SetData(material1, materialName.c_str(), material.GetSize(), 0);
        shader->SetData(lights, Light::LightName, sizeof(Light) * Light::LightsMax, 0);
        group->Bind();
        vertices->Bind();
        index->Bind();
        
        uint32_t count = index->GetCount();
        BindToRenderTexture();
        glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
        UnbindToRenderTexture();
        vertices->Unbind();
        index->Unbind();
        group->Unbind();
        for (GLuint index = 0; index < textures.size(); index++) {
            OpenGLTexture2D* texture = dynamic_cast<OpenGLTexture2D*>(material.GetTexture(textures[index].GetName()).get());
            if (texture != nullptr) {
                texture->Unbind(index);
            }
        }
        shader->Unbind();
        
        free(material1);
    }

    void OpenGLGraphics::DrawGridImpl() {
        // Shader requires a vertex array
        Ref<OpenGLVertexArray> group = CreateRef<OpenGLVertexArray>();
        
        auto view = Camera::main->GetViewMatrix();
        auto projection = Camera::main->GetProjection();
        auto position = Camera::main->GetPosition();
        auto nearZ = Camera::main->GetNearZ();
        auto farZ = Camera::main->GetFarZ();
        
        gridShader->Bind();
        gridShader->SetData(&view, "ViewUniforms", sizeof(MatrixFloat4x4), 0);
        gridShader->SetData(&projection, "ViewUniforms", sizeof(MatrixFloat4x4), sizeof(MatrixFloat4x4));
        gridShader->SetData(&position, "ViewUniforms", sizeof(Vector3Float), sizeof(MatrixFloat4x4) * 2);
        gridShader->SetData(&nearZ, "NearFarPoint", sizeof(float), 0);
        gridShader->SetData(&farZ, "NearFarPoint", sizeof(float), sizeof(float));
        group->Bind();
        BindToRenderTexture();
        glDrawArrays(GL_TRIANGLES, 0, 6);
        UnbindToRenderTexture();
        group->Unbind();
        gridShader->Unbind();
    }
}
