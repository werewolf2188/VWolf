//
//  MetalBufferGroup.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 5/6/24.
//

#include "vwpch.h"

#if defined(VWOLF_PLATFORM_MACOS) || defined(VWOLF_PLATFORM_IOS)
#include "MetalBufferGroup.h"

namespace VWolf {
    MetalBufferGroup::MetalBufferGroup(MeshData& mesh) {
        uint32_t* indices = mesh.indices.data();
        Vertex* vertices = mesh.vertices.data();

        vertexBuffer = CreateRef<MetalVertexBuffer>(vertices, sizeof(Vertex) * mesh.vertices.size());
        indexBuffer = CreateRef<MetalIndexBuffer>(indices, mesh.indices.size(), MTL::IndexTypeUInt32);
    }

    void MetalBufferGroup::SetData(MeshData& data) {
        uint32_t* indices = data.indices.data();
        Vertex* vertices = data.vertices.data();

        vertexBuffer->SetData(vertices, sizeof(Vertex) * (uint32_t)data.vertices.size());
        indexBuffer->SetData(indices, (uint32_t)data.indices.size(), MTL::IndexTypeUInt32);
    }

    MetalBufferGroup::~MetalBufferGroup() {
        
    }
}
#endif
