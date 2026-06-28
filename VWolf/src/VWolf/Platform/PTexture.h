//
//  PTexture.h
//  VWolf
//
//  Created by Enrique Moises on 6/27/26.
//

#pragma once

#include "VWolf/Core/Render/Texture.h"

namespace VWolf {
    class PTexture2D {
    public:
        PTexture2D(void * bytes, uint32_t width, uint32_t height, TextureOptions options): m_bytes(bytes), m_width(width), m_height(height), m_options(options) {}
        virtual ~PTexture2D() = default;
    public:
        uint32_t GetWidth() { return m_width; }
        uint32_t GetHeight() { return m_height; }
    public:
        virtual void* GetHandler() = 0;
    protected:
        uint32_t m_width;
        uint32_t m_height;
        TextureOptions m_options;
        
        void* m_bytes = nullptr;
    };

    class PRenderTexture {
    public:
        PRenderTexture(uint32_t width, uint32_t height, TextureOptions options = {}): m_width(width), m_height(height), m_options(options)  {}
        virtual ~PRenderTexture() = default;
    public:
        uint32_t GetWidth() { return m_width; }
        uint32_t GetHeight() { return m_height; }
    public:
        virtual void* GetHandler() = 0;
        virtual void Resize(uint32_t width, uint32_t height) = 0;
    protected:
        uint32_t m_width;
        uint32_t m_height;
        TextureOptions m_options;
    };

    class PCubemap {
    public:
        PCubemap(std::array<void *, 6> bytes, uint32_t size, TextureOptions options): m_bytes(bytes), m_size(size), m_options(options) {}
        virtual ~PCubemap() = default;
    public:
        uint32_t GetSize() { return m_size; }
        virtual void* GetHandler() = 0;
    protected:
        uint32_t m_size;
        std::array<void *, 6> m_bytes;
        TextureOptions m_options;
    };
}
