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

    struct OpenGLLight {
        Vector4Float color;
        Vector4Float position;
        Vector4Float direction;
        Vector4Float strength;
        float falloffStart;
        float falloffEnd;
        float spotPower;
        unsigned int type;
    };

    // TODO: This is not a good way of dealing with the padding, but it will work for now
    // TODO: Vertex3Float are treated as Vector4Float in GLSL (std140)
    // TODO: This only happens with lights, not with our material
    OpenGLLight CreateLightPack(Light light) {
        return {
            light.color,
            { light.position.x, light.position.y, light.position.z, 1.0f },
            { light.direction.x, light.direction.y, light.direction.z, 0.0f },
            { light.strength.x, light.strength.y, light.strength.z, 0.0f },
            light.falloffStart,
            light.falloffEnd,
            light.spotPower,
            (unsigned int)light.type
        };
    }

    void OpenGLRenderer::ProcessItems() {
        std::vector<Ref<BufferGroup>> groups;
        for (auto renderItem: items) {
            auto data = renderItem->data.GetVertices();
            auto indices = renderItem->data.indices;
            Ref<OpenGLVertexBuffer> vertices = CreateRef<OpenGLVertexBuffer>(m_window, data.data(), data.size() * sizeof(float));
            vertices->SetLayout(MeshData::Layout);
            Ref<OpenGLIndexBuffer> index = CreateRef<OpenGLIndexBuffer>(m_window, indices.data(), indices.size());
            Ref<OpenGLVertexArray> group = CreateRef<OpenGLVertexArray>(m_window);
            group->AddVertexBuffer(vertices);
            group->SetIndexBuffer(index);
            groups.push_back(group);
        }
        
        CameraPass projection = GetCameraPass();
        
        
        if (clearColor)
            glClearColor(backgroundColor.r, backgroundColor.b, backgroundColor.g, backgroundColor.a);
        if (clearDepthStencil)
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (int i = 0; i < items.size(); i++) {
            Ref<Shader> shader = ShaderLibrary::GetShader(items[i]->material.GetShader().c_str());
            void* material = items[i]->material.GetDataPointer();
            shader->Bind();
            shader->SetData(&projection, ShaderLibrary::CameraBufferName, sizeof(CameraPass), 0);
            shader->SetData(&items[i]->transform, ShaderLibrary::ObjectBufferName, sizeof(MatrixFloat4x4), 0);
            shader->SetData(material, items[i]->material.GetName(), items[i]->material.GetSize(), 0);
            // TODO: I'm not sending the entire data for light. The alignment is not working well in OpenGL
            OpenGLLight lig = CreateLightPack(items[i]->light);
            shader->SetData(&lig, Light::LightName, sizeof(OpenGLLight), 0);
            groups[i]->Bind();
            uint32_t count = groups[i]->GetIndexBuffer()->GetCount();
            glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
            groups[i]->Unbind();
            shader->Unbind();
        }

        clearColor = false;
        clearDepthStencil = false;
    }
}
