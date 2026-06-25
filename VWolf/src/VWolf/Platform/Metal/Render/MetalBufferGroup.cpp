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
    MetalBufferGroup::MetalBufferGroup(Ref<Mesh> mesh) {
        std::vector<uint32_t> indices = mesh->GetTriangles();
        std::vector<float> vertices = mesh->GetNativeVector();

        vertexBuffer = CreateRef<MetalVertexBuffer>(vertices.data(), sizeof(float) * vertices.size());
        indexBuffer = CreateRef<MetalIndexBuffer>(indices.data(), indices.size(), MTL::IndexTypeUInt32);
    }

    void MetalBufferGroup::SetData(Ref<Mesh> mesh) {
        std::vector<uint32_t> indices = mesh->GetTriangles();
        std::vector<float> vertices = mesh->GetNativeVector();

        vertexBuffer = CreateRef<MetalVertexBuffer>(vertices.data(), sizeof(float) * vertices.size());
        indexBuffer = CreateRef<MetalIndexBuffer>(indices.data(), indices.size(), MTL::IndexTypeUInt32);
    }

    MetalBufferGroup::~MetalBufferGroup() {
        
    }
}
#endif
