//
//  Texture.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 1/25/23.
//

#include "vwpch.h"
#include "VWolf/Platform/PTexture.h"
#include "VWolf/Core/Application.h"

#include "VWolf/Platform/OpenGL/Render/OpenGLTexture.h"
#ifdef VWOLF_PLATFORM_WINDOWS
#include "VWolf/Platform/DirectX12/Render/DirectX12Texture.h"
#endif
#if defined(VWOLF_PLATFORM_MACOS) || defined(VWOLF_PLATFORM_IOS)
#include "VWolf/Platform/Metal/Render/MetalTexture.h"
#endif


namespace VWolf {

    Ref<PTexture2D> LoadTexture2D(TextureDefault textureDefault, uint32_t width, uint32_t height, TextureOptions options) {
        Ref<PTexture2D> texture;
        switch(Application::GetApplication()->GetDriverType()) {
            case DriverType::OpenGL:
                texture = CreateRef<OpenGLTexture2D>(textureDefault, width, height, options);
                break;
#ifdef VWOLF_PLATFORM_WINDOWS
            case DriverType::DirectX12:
                texture = CreateRef<DirectX12Texture2D>(textureDefault, width,  height, options);
                break;
#endif
#if defined(VWOLF_PLATFORM_MACOS) || defined(VWOLF_PLATFORM_IOS)
            case DriverType::Metal:
                texture = CreateRef<MetalTexture2D>(textureDefault, width, height, options);
                break;
#endif
            default:
                VWOLF_CORE_ASSERT(false, "Texture: Not yet implemented");
        }
        return texture;
    }

    Ref<PTexture2D> LoadTexture2D(const std::string filePath, TextureOptions options) {
        Ref<PTexture2D> texture;
        switch(Application::GetApplication()->GetDriverType()) {
            case DriverType::OpenGL:
                texture = CreateRef<OpenGLTexture2D>(filePath, options);
                break;
#ifdef VWOLF_PLATFORM_WINDOWS
            case DriverType::DirectX12:
                texture = CreateRef<DirectX12Texture2D>(filePath, options);
                break;
#endif
#if defined(VWOLF_PLATFORM_MACOS) || defined(VWOLF_PLATFORM_IOS)
            case DriverType::Metal:
                texture = CreateRef<MetalTexture2D>(filePath, options);
                break;
#endif
            default:
                VWOLF_CORE_ASSERT(false, "Texture: Not yet implemented");
        }
        return texture;
    }

    void* GetHandlerForTexture2D(Ref<PTexture2D> texture2d) {
        return texture2d->GetHandler();
    }

    Ref<PRenderTexture> LoadRenderTexture(uint32_t width, uint32_t height, TextureOptions options) {
        Ref<PRenderTexture> texture;
        switch(Application::GetApplication()->GetDriverType()) {
            case DriverType::OpenGL:
                texture = CreateRef<OpenGLRenderTexture>(width, height, false, options);
                break;
#ifdef VWOLF_PLATFORM_WINDOWS
            case DriverType::DirectX12:
                texture = CreateRef<DirectX12RenderTexture>(width, height, false, options);
                break;
#endif
#if defined(VWOLF_PLATFORM_MACOS) || defined(VWOLF_PLATFORM_IOS)
            case DriverType::Metal:
                texture = CreateRef<MetalRenderTexture>(width, height, false, options);
                break;
#endif
            default:
                VWOLF_CORE_ASSERT(false, "Render Texture: Not yet implemented");
        }
        return texture;
    }

    void* GetHandlerForRenderTexture(Ref<PRenderTexture> renderTexture) {
        return renderTexture->GetHandler();
    }

    void ResizeRenderTexture(Ref<PRenderTexture> renderTexture, uint32_t width, uint32_t height) {
        renderTexture->Resize(width, height);
    }

    Ref<PCubemap> LoadCubemap(TextureDefault textureDefault, uint32_t size, TextureOptions options) {
        Ref<PCubemap> texture;
        switch(Application::GetApplication()->GetDriverType()) {
            case DriverType::OpenGL:
                texture = CreateRef<OpenGLCubemap>(textureDefault, size, options);
                break;
#ifdef VWOLF_PLATFORM_WINDOWS
            case DriverType::DirectX12:
                texture = CreateRef<DirectX12Cubemap>(textureDefault, size, options);
                break;
#endif
#if defined(VWOLF_PLATFORM_MACOS) || defined(VWOLF_PLATFORM_IOS)
            case DriverType::Metal:
                texture = CreateRef<MetalCubemap>(textureDefault, size, options);
                break;
#endif
            default:
                VWOLF_CORE_ASSERT(false, "Texture: Not yet implemented");
        }
        return texture;
    }

    Ref<PCubemap> LoadCubemap(std::filesystem::path path, TextureOptions options) {
        Ref<PCubemap> texture;
        switch(Application::GetApplication()->GetDriverType()) {
            case DriverType::OpenGL:
                texture = CreateRef<OpenGLCubemap>(path, options);
                break;
    #ifdef VWOLF_PLATFORM_WINDOWS
            case DriverType::DirectX12:
                texture = CreateRef<DirectX12Cubemap>(path, options);
                break;
    #endif
    #if defined(VWOLF_PLATFORM_MACOS) || defined(VWOLF_PLATFORM_IOS)
            case DriverType::Metal:
                texture = CreateRef<MetalCubemap>(path, options);
                break;
    #endif
            default:
                VWOLF_CORE_ASSERT(false, "Texture: Not yet implemented");
        }
        return texture;
    }

    void* GetHandlerForRenderTexture(Ref<PCubemap> cubemap) {
        return cubemap->GetHandler();
    }
}
