//
//  MetalBuffer.hpp
//  VWolf
//
//  Created by Enrique Ricalde on 5/6/24.
//

#pragma once

#include "VWolf/Platform/Metal/Core/Core.h"

namespace VWolf {
    class MetalVertexBuffer
    {
    public:
        MetalVertexBuffer(uint32_t size);
        MetalVertexBuffer(void* vertices, uint32_t size);
        ~MetalVertexBuffer();
    public:
        uint32_t GetSize() const { return size; }
        MTL::Buffer* GetBuffer() { return vertexData; }
        operator MTL::Buffer* () { return vertexData; }
    public:
        void SetData(const void* data, uint32_t size);
    private:
        MTL::Buffer* vertexData;
        uint32_t size;
    };

    class MetalIndexBuffer
    {
    public:
        MetalIndexBuffer(uint32_t* indices, uint32_t count, MTL::IndexType type);
        ~MetalIndexBuffer();
    public:
        uint32_t GetCount() const { return count; }
        MTL::IndexType GetType() const { return type; }
        MTL::Buffer* GetBuffer() { return indexData; }
        operator MTL::Buffer*() { return indexData; }
    public:
        void SetData(const uint32_t* indices, uint32_t count, MTL::IndexType type);
    private:
        MTL::Buffer* indexData;
        MTL::IndexType type;
        uint32_t count;
    };
}
