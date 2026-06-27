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
        PTexture2D(TextureDefault textureDefault, uint32_t width, uint32_t height, TextureOptions options = {}): m_textureDefault(textureDefault), m_width(width), m_height(height), m_options(options) {}
        PTexture2D(const std::string filePath, TextureOptions options = {}): m_options(options) {}
        virtual ~PTexture2D() = default;
    public:
        uint32_t GetWidth() { return m_width; }
        uint32_t GetHeight() { return m_height; }
    public:
        virtual void* GetHandler() = 0;
    protected:
        virtual void PopulateColor() = 0;
    protected:
        uint32_t m_width;
        uint32_t m_height;
        TextureDefault m_textureDefault;
        TextureOptions m_options;
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
        PCubemap(TextureDefault textureDefault, uint32_t size, TextureOptions options = {}): m_textureDefault(textureDefault), m_size(size), m_options(options) {}
        PCubemap(std::filesystem::path path, TextureOptions options = {}): m_options(options) {}
        virtual ~PCubemap() = default;
    public:
        uint32_t GetSize() { return m_size; }
        virtual void* GetHandler() = 0;
    protected:
        virtual void PopulateColor() = 0;
    protected:
        uint32_t m_size;
        TextureDefault m_textureDefault;
        TextureOptions m_options;
    };

    // Define the 6 cubemap faces
    enum class CubemapFace {
        FACE_RIGHT = 0,  // +X
        FACE_LEFT = 1,   // -X
        FACE_TOP = 2,    // +Y
        FACE_BOTTOM = 3, // -Y
        FACE_FRONT = 4,  // +Z
        FACE_BACK = 5    // -Z
    };

}
