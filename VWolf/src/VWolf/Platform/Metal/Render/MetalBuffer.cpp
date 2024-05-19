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
    MetalVertexBuffer::MetalVertexBuffer(uint32_t size): size(size), maxSize(size) {
        vertexData = MetalDriver::GetCurrent()->GetDevice()->GetDevice()->newBuffer(size, {});
    }

    MetalVertexBuffer::MetalVertexBuffer(void* vertices, uint32_t size): size(size), maxSize(size) {
        vertexData = MetalDriver::GetCurrent()->GetDevice()->GetDevice()->newBuffer(vertices, size, {});
    }

    MetalVertexBuffer::~MetalVertexBuffer() {
        if (vertexData != nullptr)
            vertexData->release();
    }

    void MetalVertexBuffer::SetData(const void* data, uint32_t size) {
        // TODO: Bug here. Resizing is not working properly
        if (size > this->maxSize) {
            this->maxSize = size;
            if (vertexData != nullptr) {
                vertexData->release();
                vertexData = nullptr;
            }
    
            vertexData = MetalDriver::GetCurrent()->GetDevice()->GetDevice()->newBuffer(data, size, {});
            return;
        }

        this->size = size;
        memcpy(vertexData->contents(), data, size);
    }

    MetalIndexBuffer::MetalIndexBuffer(uint32_t* indices, uint32_t count, MTL::IndexType type): count(count), type(type), maxCount(count) {
        indexData = MetalDriver::GetCurrent()->GetDevice()->GetDevice()->newBuffer(indices,
                                                                                   (type == MTL::IndexTypeUInt32 ? sizeof(uint32_t): sizeof(uint16_t)) * count, {});
    }

    MetalIndexBuffer::~MetalIndexBuffer() {
        if (indexData != nullptr)
            indexData->release();
    }

    void MetalIndexBuffer::SetData(const uint32_t* indices, uint32_t count, MTL::IndexType type) {
        // TODO: Bug here. Resizing is not working properly
        if (count > this->maxCount || type != this->type) {
            this->maxCount = count;
            this->type = type;
            if (indexData != nullptr) {
                indexData->release();
                indexData = nullptr;
            }
            indexData = MetalDriver::GetCurrent()->GetDevice()->GetDevice()->newBuffer(indices,
                                                                                       (type == MTL::IndexTypeUInt32 ? sizeof(uint32_t): sizeof(uint16_t)) * count, {});
            return;
        }
        this->count = count;
        memcpy(indexData->contents(), indices,
               (type == MTL::IndexTypeUInt32 ? sizeof(uint32_t): sizeof(uint16_t)) * count);
    }
}
#endif
