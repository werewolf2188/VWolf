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
    void OpenGLRenderer::ProcessItems() {
        
        std::vector<Ref<OpenGLVertexArray>> groups;
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
            Light* lights = this->lights.data();
            shader->Bind();
            shader->SetData(&projection, ShaderLibrary::CameraBufferName, sizeof(CameraPass), 0);
            shader->SetData(&items[i]->transform, ShaderLibrary::ObjectBufferName, sizeof(MatrixFloat4x4), 0);
            shader->SetData(material, items[i]->material.GetName(), items[i]->material.GetSize(), 0);
            shader->SetData(lights, Light::LightName, sizeof(Light) * Light::LightsMax, 0);
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
