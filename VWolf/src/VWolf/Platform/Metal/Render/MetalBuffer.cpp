//
//  MetalBuffer.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 5/6/24.
//

#include "vwpch.h"

#if defined(VWOLF_PLATFORM_MACOS) || defined(VWOLF_PLATFORM_IOS)
#include "MetalBuffer.h"

#include "VWolf/Platform/Metal/MetalDriver.h"

namespace VWolf {
    MetalVertexBuffer::MetalVertexBuffer(uint32_t size): size(size) {
        vertexData = MetalDriver::GetCurrent()->GetDevice()->GetDevice()->newBuffer(size, {});
    }

    MetalVertexBuffer::MetalVertexBuffer(void* vertices, uint32_t size): size(size) {
        vertexData = MetalDriver::GetCurrent()->GetDevice()->GetDevice()->newBuffer(vertices, size, {});
    }

    MetalVertexBuffer::~MetalVertexBuffer() {
        if (vertexData != nullptr)
            vertexData->release();
    }

    void MetalVertexBuffer::SetData(const void* data, uint32_t size) {
        if (size > this->size) {
            this->size = size;
            if (vertexData != nullptr) {
                vertexData->release();
                vertexData = nullptr;
            }
    
            vertexData = MetalDriver::GetCurrent()->GetDevice()->GetDevice()->newBuffer(data, size, {});
            return;
        }
        memcpy(vertexData->contents(), data, size);
    }

    MetalIndexBuffer::MetalIndexBuffer(uint32_t* indices, uint32_t count, MTL::IndexType type): count(count), type(type) {
        indexData = MetalDriver::GetCurrent()->GetDevice()->GetDevice()->newBuffer(indices,
                                                                                   (type == MTL::IndexTypeUInt32 ? sizeof(uint32_t): sizeof(uint16_t)) * count, {});
    }

    MetalIndexBuffer::~MetalIndexBuffer() {
        if (indexData != nullptr)
            indexData->release();
    }

    void MetalIndexBuffer::SetData(const uint32_t* indices, uint32_t count, MTL::IndexType type) {
        if (count > this->count || type != this->type) {
            this->count = count;
            this->type = type;
            if (indexData != nullptr) {
                indexData->release();
                indexData = nullptr;
            }
            indexData = MetalDriver::GetCurrent()->GetDevice()->GetDevice()->newBuffer(indices,
                                                                                       (type == MTL::IndexTypeUInt32 ? sizeof(uint32_t): sizeof(uint16_t)) * count, {});
            return;
        }
        memcpy(indexData->contents(), indices,
               (type == MTL::IndexTypeUInt32 ? sizeof(uint32_t): sizeof(uint16_t)) * count);
    }
}
#endif
