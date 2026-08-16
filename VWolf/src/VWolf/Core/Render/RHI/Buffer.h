//
//  Buffer.h
//  VWolf
//
//  Created by Enrique Moises on 8/19/26.
//

#pragma once

#include "Types.h"
#include "Resource.h"

namespace VWolf::RHI {
    enum class BufferType {
        Vertex,
        Index,
        Constant
    };

    class BufferSettings {
    public:
        
        static BufferSettings ForVertextBuffer(uint32_t size) {
            return { BufferType::Vertex, PixelFormat::Unknown, size, 0 };
        }
        
        static BufferSettings ForIndexBuffer(uint32_t count, PixelFormat format) {
            uint32_t size;
            switch (format) {
                case PixelFormat::R32Uint:
                    size = count * sizeof(uint32_t);
                    break;
                case PixelFormat::R16Uint:
                    size = count * sizeof(uint16_t);
                    break;
                default: throw std::invalid_argument("Indexes only accept R132Uint or R16Uint");
            }
            return { BufferType::Index, format, size, count };
        }
        
        static BufferSettings ForConstantBuffer(uint32_t size) {
            return { BufferType::Constant, PixelFormat::Unknown, size, 0 };
        }
        
        const BufferType& GetType() const { return type; }
        const PixelFormat& GetFormat() const { return format; }
        const uint32_t& GetSize() const { return size; }
        const uint32_t& GetCount() const { return count; }
    protected:
    private:
        BufferSettings(BufferType type,
                       PixelFormat format,
                       uint32_t size,
                       uint32_t count): type(type), format(format), size(size), count(count) {}
        
        BufferType type;
        PixelFormat format;
        uint32_t size;
        uint32_t count;
    };

    class Buffer: public Resource {
    public:
        Buffer(const BufferSettings& settings): settings(settings) {}
        
        virtual void SetData(const SubResource& subResource) = 0;
        
        virtual const ResourceType GetResourceType() const override { return ResourceType::Buffer; }
        
        const BufferSettings& GetSettings() const { return settings; }
    protected:
        BufferSettings settings;
    private:
    };
}
