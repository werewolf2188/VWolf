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

    // TODO: This is not a good way of dealing with the padding, but it will work for now
    // TODO: Vertex3Float are treated as Vector4Float in GLSL (std140)
    // TODO: This only happens with lights, not with our material
    float* CreateLightPack(Light light) {
        float* pack = new float[20];
        pack[0] = light.color.r;
        pack[1] = light.color.g;
        pack[2] = light.color.b;
        pack[3] = light.color.a;
        pack[4] = light.position.x;
        pack[5] = light.position.y;
        pack[6] = light.position.z;
        pack[7] = 0.0f;
        pack[8] = light.direction.x;
        pack[9] = light.direction.y;
        pack[10] = light.direction.z;
        pack[11] = 0.0f;
        pack[12] = light.strength.x;
        pack[13] = light.strength.y;
        pack[14] = light.strength.z;
        pack[15] = 0.0f;
        pack[16] = light.falloffStart;
        pack[17] = light.falloffEnd;
        pack[18] = light.spotPower;
        pack[19] = (unsigned int)light.type;
        return pack;
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
            shader->SetData(CreateLightPack(items[i]->light), Light::LightName, sizeof(Light), 0);
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
