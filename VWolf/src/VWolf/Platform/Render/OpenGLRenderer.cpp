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
        std::vector<Ref<BufferGroup>> groups;
        for (auto renderItem: items) {
            auto data = renderItem->data.GetVertices();
            auto indices = renderItem->data.indices;
            Ref<VertexBuffer> vertices = VertexBuffer::Create(data.data(), data.size() * sizeof(float));
            vertices->SetLayout(MeshData::Layout);
            Ref<IndexBuffer> index = IndexBuffer::Create(indices.data(), indices.size());
            Ref<BufferGroup> group = BufferGroup::Create();
            group->AddVertexBuffer(vertices);
            group->SetIndexBuffer(index);
            groups.push_back(group);
        }
        
        projection = m_camera->GetProjection();
        Ref<Shader> shader = ShaderLibrary::GetShader(shaderName.c_str());
        m_renderApi->Begin(*m_camera.get(), shader);
        m_renderApi->ClearColor(backgroundColor);
        m_renderApi->Clear();
        shader->Bind();
        shader->SetData(&projection, "Camera", sizeof(VWolf::MatrixFloat4x4), 0);
        for (int i = 0; i < items.size(); i++) {
            shader->SetData(&items[i]->transform, "Object", sizeof(VWolf::MatrixFloat4x4), 0);
            groups[i]->Bind();
            m_renderApi->DrawIndexed(groups[i], 0);
            groups[i]->Unbind();
        }
        shader->Unbind();
        m_renderApi->End();
    }
}
