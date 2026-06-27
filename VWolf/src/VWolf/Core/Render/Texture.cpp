//
//  Texture.cpp
//  VWolf
//
//  Created by Enrique Moises on 6/26/26.
//

#include "Texture.h"

namespace VWolf {
    VWOLF_CREATE_CONVERT_GENERIC_ENUM_ENCODER(TextureWrapMode, None, Repeat, Clamp, Mirror, MirrorOnce);
    VWOLF_CREATE_CONVERT_GENERIC_ENUM_ENCODER(TextureFilterMode, Point, Bilinear, Trilinear);
    VWOLF_CREATE_CONVERT_GENERIC_CLASS_ENCODER(TextureOptions);

    extern Ref<PTexture2D> LoadTexture2D(TextureDefault textureDefault, uint32_t width, uint32_t height, TextureOptions options);
    extern Ref<PTexture2D> LoadTexture2D(const std::string filePath, TextureOptions options);
    extern void* GetHandlerForTexture2D(Ref<PTexture2D> texture2d);

    extern Ref<PRenderTexture> LoadRenderTexture(uint32_t width, uint32_t height, TextureOptions options);
    extern void* GetHandlerForRenderTexture(Ref<PRenderTexture> renderTexture);
    extern void ResizeRenderTexture(Ref<PRenderTexture> renderTexture, uint32_t width, uint32_t height);

    extern Ref<PCubemap> LoadCubemap(TextureDefault textureDefault, uint32_t size, TextureOptions options);
    extern Ref<PCubemap> LoadCubemap(std::filesystem::path path, TextureOptions options);
    extern void* GetHandlerForRenderTexture(Ref<PCubemap> cubemap);

    Texture2D::Texture2D(UUID _id, TextureDefault textureDefault, uint32_t width, uint32_t height, TextureOptions options): m_textureDefault(textureDefault), m_width(width), m_height(height), Texture(_id, options) {
        _innerTexture = LoadTexture2D(textureDefault, width, height, options);
    }

    Texture2D::Texture2D(UUID _id, const std::string filePath, TextureOptions options): Texture(_id, options) {
        _innerTexture = LoadTexture2D(filePath, options);
    }

    void* Texture2D::GetHandler() {
        return GetHandlerForTexture2D(_innerTexture);
    }

    Ref<Texture2D> Texture2D::Load(UUID _id, TextureDefault textureDefault, uint32_t width, uint32_t height, TextureOptions options) {
        Ref<Texture2D> tex = CreateRef<Texture2D>(_id, textureDefault, width, height, options);
        ObjectResourceManager::AddObject(_id, tex);
        return tex;
    }

    Ref<Texture2D> Texture2D::Load(UUID _id, const std::string filePath, TextureOptions options) {
        Ref<Texture2D> tex = CreateRef<Texture2D>(_id, filePath, options);
        ObjectResourceManager::AddObject(_id, tex);
        return tex;
    }

    RenderTexture::RenderTexture(uint32_t width, uint32_t height, TextureOptions options): m_width(width), m_height(height), Texture(UUID::NewUUID(), options)  {
        _innerTexture = LoadRenderTexture(width, height, options);
    }

    void* RenderTexture::GetHandler() {
        return GetHandlerForRenderTexture(_innerTexture);
    }

    void RenderTexture::Resize(uint32_t width, uint32_t height) {
        m_width = width;
        m_height = height;
        ResizeRenderTexture(_innerTexture, width, height);
    }

    Cubemap::Cubemap(UUID _id, TextureDefault textureDefault, uint32_t size, TextureOptions options): m_textureDefault(textureDefault), m_size(size), Texture(_id, options) {
        _innerTexture = LoadCubemap(textureDefault, size, options);
    }

    Cubemap::Cubemap(UUID _id, std::filesystem::path path, TextureOptions options): Texture(_id, options) {
        _innerTexture = LoadCubemap(path, options);
    }

    void* Cubemap::GetHandler() {
        return GetHandlerForRenderTexture(_innerTexture);
    }

    Ref<Cubemap> Cubemap::Load(UUID _id, TextureDefault textureDefault, uint32_t size, TextureOptions options) {
        Ref<Cubemap> tex = CreateRef<Cubemap>(_id, textureDefault, size, options);
        ObjectResourceManager::AddObject(_id, tex);
        return tex;
    }

    Ref<Cubemap> Cubemap::Load(UUID _id, std::filesystem::path path, TextureOptions options) {
        Ref<Cubemap> tex = CreateRef<Cubemap>(_id, path, options);
        ObjectResourceManager::AddObject(_id, tex);
        return tex;
    }
}
