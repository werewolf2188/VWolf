//
//  OpenGLRenderer.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 10/11/22.
//

#include "vwpch.h"
#include "OpenGLGraphics.h"
#include "HLSLOpenGLShader.h"

#include "VWolf/Core/Render/RenderItem.h"
#include "VWolf/Core/Render/GraphicsContext.h"

#include "VWolf/Platform/OpenGL/Core/GLCore.h"

namespace VWolf {
    static GLenum GetTopology(Topology topology) {
        switch (topology) {
            case Topology::Triangles:
                return GL_TRIANGLES;
            case Topology::Quads:
                return GL_QUADS;
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
        shadowMap = CreateRef<OpenGLRenderTexture>(1024, 1024, true, TextureOptions());
        emptyShadowMap = std::dynamic_pointer_cast<OpenGLTexture2D>(CreateRef<Texture2D>(UUID::NewUUID(), TextureDefault::White, 1024, 1024, TextureOptions())->GetInnerTexture());
    }

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
    }

    void OpenGLGraphics::BeginFrameImpl()
    {

    }

    void OpenGLGraphics::EndFrameImpl()
    {

    }

    void OpenGLGraphics::BeginSceneImpl() {

    }

    void OpenGLGraphics::EndSceneImpl() {
        useRenderTexture = true;
        
        ClearImpl();
        DrawShadowMap();
        DrawQueue();
        DrawPostProcess();
        useRenderTexture = false;
    }

    void OpenGLGraphics::SetRenderTextureImpl(Ref<RenderTexture> renderTexture)
    {
        this->renderTexture = renderTexture->GetInnerTexture();
    }

    void OpenGLGraphics::DrawShadowMap() {
        shadowMap->Bind();
        GLThrowIfFailed(glClear(GL_DEPTH_BUFFER_BIT));
        HLSLOpenGLShader* shader = (HLSLOpenGLShader*) Shader::GetShader("Shadow")->GetInternalShader().get();
        for (const Matrix4x4& viewProjection: GraphicsContext::GetLightsSpaces()) {

            for(Ref<GraphicsCommand> command: GraphicsContext::GetList().GetCommands()) {
                Ref<DrawMeshCommand> drawMeshCommand = std::dynamic_pointer_cast<DrawMeshCommand>(command);
                if (!drawMeshCommand->GetCastShadows()) continue;
                
                Ref<Mesh> mesh1 = drawMeshCommand->GetMesh();
                
                if (mesh1 == nullptr || mesh1->GetVertices().size() == 1) return;; // It's a light

                mesh1->BuildVertexBuffer(shader->GetAttributes());
                
                auto data = mesh1->GetNativeVector();
                if (data.size() == 1) continue;; // It's a light
                auto indices = mesh1->GetTriangles();
                
                Ref<OpenGLVertexBuffer> vertices = CreateRef<OpenGLVertexBuffer>(data.data(), data.size() * sizeof(float));
                Ref<OpenGLIndexBuffer> index = CreateRef<OpenGLIndexBuffer>(indices.data(), indices.size());
                Ref<OpenGLVertexArray> group = CreateRef<OpenGLVertexArray>(vertices, shader->GetAttributes());

                
                Matrix4x4 transform = drawMeshCommand->GetTransform();
                
                shader->Bind();
                shader->SetData(&viewProjection, Shader::CameraBufferName, sizeof(Matrix4x4), 0);
                shader->SetData(&transform, Shader::ObjectBufferName, sizeof(Matrix4x4), 0);

                group->Bind();
                vertices->Bind();
                index->Bind();
                
                uint32_t count = index->GetCount();
                GLThrowIfFailed(glDrawElements(GetTopology(mesh1->GetSubMesh(0).GetTopology()), count, GL_UNSIGNED_INT, nullptr));

                vertices->Unbind();
                index->Unbind();
                group->Unbind();
                shader->Unbind();
            }
        }
        shadowMap->Unbind();
    }

    void OpenGLGraphics::DrawQueue() {
        for(Ref<GraphicsCommand> command: GraphicsContext::GetList().GetCommands()) {
            Ref<DrawMeshCommand> drawMeshCommand = std::dynamic_pointer_cast<DrawMeshCommand>(command);
            
            Ref<Mesh> mesh1 = drawMeshCommand->GetMesh();
            Ref<Material> material = drawMeshCommand->GetMaterial();
            Ref<Camera> camera = drawMeshCommand->GetCamera();
            Matrix4x4 transform = drawMeshCommand->GetTransform();
            
            if (mesh1 == nullptr || mesh1->GetVertices().size() == 1) continue; // It's a light
            
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

            Ref<HLSLOpenGLShader> shader = std::dynamic_pointer_cast<HLSLOpenGLShader>(Shader::GetShader(material->GetShaderName().c_str())->GetInternalShader());
            void* material1 = material->GetDataPointer();
            Light* lights = GraphicsContext::GetLights().data();
            std::vector<ShaderInput> textures = shader->GetTextureInputs();
            shader->Bind();
            for (GLuint index = 0; index < textures.size(); index++) {
                if (textures[index].GetName() == "Shadow") {
                    shadowMap->DepthTextureBind(index);
                    GLThrowIfFailed(glUniform1i(textures[index].GetIndex(), index));
                } else {
                    Texture* tex = material->GetTexture(textures[index].GetName()).get();
                    OpenGLBindableTexture* texture;
                    if (dynamic_cast<Texture2D *>(tex) != nullptr) {
                        texture = dynamic_cast<OpenGLBindableTexture*>(dynamic_cast<Texture2D *>(tex)->GetInnerTexture().get());
                    }
                    else if (dynamic_cast<Cubemap *>(tex) != nullptr) {
                        texture = dynamic_cast<OpenGLBindableTexture*>(dynamic_cast<Cubemap *>(tex)->GetInnerTexture().get());
                    }
                    else { continue; }
                    if (texture != nullptr) {
                        texture->Bind(index);
                        // TODO: Move this inside bind
                        GLThrowIfFailed(glUniform1i(textures[index].GetIndex(), index));
                    }
                }
            }
            shader->SetData(&cameraPass, Shader::CameraBufferName, sizeof(CameraPass), 0);
            shader->SetData(&transform, Shader::ObjectBufferName, sizeof(Matrix4x4), 0);
            shader->SetData(material1, materialName.c_str(), material->GetSize(), 0);
            if (lights) {
                shader->SetData(lights, Light::LightName, sizeof(Light) * Light::LightsMax, 0);
                Matrix4x4* spacesPointer = GraphicsContext::GetLightsSpaces().data();
                shader->SetData(spacesPointer, Light::LightSpaceName, sizeof(Matrix4x4) * Light::LightsMax, 0);
            }
            mesh1->BuildVertexBuffer(shader->GetAttributes());
            
            auto data = mesh1->GetNativeVector();
            auto indices = mesh1->GetTriangles();
            Ref<OpenGLVertexBuffer> vertices = CreateRef<OpenGLVertexBuffer>(data.data(), data.size() * sizeof(float));
            Ref<OpenGLIndexBuffer> index = CreateRef<OpenGLIndexBuffer>(indices.data(), indices.size());
            Ref<OpenGLVertexArray> group = CreateRef<OpenGLVertexArray>(vertices, shader->GetAttributes());
            
            group->Bind();
            vertices->Bind();
            index->Bind();
            
            uint32_t count = index->GetCount();
            BindToRenderTexture();
            GLThrowIfFailed(glDrawElements(GetTopology(mesh1->GetSubMesh(0).GetTopology()), count, GL_UNSIGNED_INT, nullptr));
            UnbindToRenderTexture();
            vertices->Unbind();
            index->Unbind();
            group->Unbind();
            for (GLuint index = 0; index < textures.size(); index++) {
                if (textures[index].GetName() == "Shadow") {
                    shadowMap->DepthTextureUnbind(index);
                } else {
                    Texture* tex = material->GetTexture(textures[index].GetName()).get();
                    OpenGLBindableTexture* texture;
                    if (dynamic_cast<Texture2D *>(tex) != nullptr) {
                        texture = dynamic_cast<OpenGLBindableTexture*>(dynamic_cast<Texture2D *>(tex)->GetInnerTexture().get());
                    }
                    else if (dynamic_cast<Cubemap *>(tex) != nullptr) {
                        texture = dynamic_cast<OpenGLBindableTexture*>(dynamic_cast<Cubemap *>(tex)->GetInnerTexture().get());
                    }
                    else { continue; }
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
