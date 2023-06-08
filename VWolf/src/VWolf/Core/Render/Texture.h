//
//  Texture.hpp
//  VWolf
//
//  Created by Enrique Ricalde on 1/25/23.
//

#pragma once
#include "VWolf/Core/Base.h"
namespace VWolf {
    class Texture2D;
    class RenderTexture;
    class Cubemap;

    enum class TextureWrapMode {
        None, Repeat, Clamp, Mirror, MirrorOnce
    };

    enum class TextureFilterMode {
        Point, Bilinear, Trilinear
    };

    class TextureOptions {
    public:
        TextureFilterMode GetFilterMode() const { return filterMode; }
        TextureWrapMode GetWrapModeU() const { return wrapModeU; }
        TextureWrapMode GetWrapModeV() const { return wrapModeV; }
        TextureWrapMode GetWrapModeW() const { return wrapModeW; }
        TextureWrapMode GetWrapMode() const { return wrapMode; }
#if defined(DEBUG) || defined(_DEBUG)
        bool IsTestTexture() const { return true; }
#endif
    private:
        TextureFilterMode filterMode = TextureFilterMode::Point;
        TextureWrapMode wrapModeU = TextureWrapMode::None;
        TextureWrapMode wrapModeV = TextureWrapMode::None;
        TextureWrapMode wrapModeW = TextureWrapMode::None;
        TextureWrapMode wrapMode = TextureWrapMode::Repeat;
#if defined(DEBUG) || defined(_DEBUG)
        bool testTexture = true;
#endif
    };

    class Texture {
    public:
        Texture(TextureOptions options = {}): m_options(options) {}
        virtual ~Texture() = default;

        virtual void* GetHandler() = 0;
    public:
        static Ref<Texture2D> LoadTexture2D(uint32_t width, uint32_t height, TextureOptions options = {});
        static Ref<Texture2D> LoadTexture2D(const std::string filePath, TextureOptions options = {});
        static Ref<RenderTexture> LoadRenderTexture(uint32_t width, uint32_t height, TextureOptions options = {});
        static Ref<Cubemap> LoadCubemap(uint32_t size, TextureOptions options = {});
        static Ref<Cubemap> LoadCubemap(std::array<std::string, 6> paths, TextureOptions options = {});
    public:
        TextureOptions GetOptions() { return m_options; }
    protected:
        TextureOptions m_options;
    };

    class Texture2D: public Texture {
    public:
        Texture2D(uint32_t width, uint32_t height, TextureOptions options = {}): m_width(width), m_height(height), Texture(options) {}
        Texture2D(const std::string filePath, TextureOptions options = {}): Texture(options) {}
        virtual ~Texture2D() = default;
    public:
        uint32_t GetWidth() { return m_width; }
        uint32_t GetHeight() { return m_height; }
    protected:
        uint32_t m_width;
        uint32_t m_height;
    };

    class RenderTexture: public Texture {
    public:
        RenderTexture(uint32_t width, uint32_t height, TextureOptions options = {}): m_width(width), m_height(height), Texture(options)  {}
        virtual ~RenderTexture() = default;
    public:
        uint32_t GetWidth() { return m_width; }
        uint32_t GetHeight() { return m_height; }

        virtual void Resize(uint32_t width, uint32_t height) = 0;
    protected:
        uint32_t m_width;
        uint32_t m_height;
    };

    class Cubemap: public Texture {
    public:
        Cubemap(uint32_t size, TextureOptions options = {}): m_size(size), Texture(options) {}
        Cubemap(std::array<std::string, 6> paths, TextureOptions options = {}): Texture(options) {}
        virtual ~Cubemap() = default;
    public:
        uint32_t GetSize() { return m_size; }
    protected:
        uint32_t m_size;
    };
}

