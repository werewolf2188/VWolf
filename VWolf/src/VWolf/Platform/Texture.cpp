//
//  Texture.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 1/25/23.
//

#include "vwpch.h"
#include "VWolf/Core/Render/Texture.h"
#include "VWolf/Core/Application.h"

#include "VWolf/Platform/OpenGL/Render/OpenGLTexture.h"
#ifdef VWOLF_PLATFORM_WINDOWS
#include "VWolf/Platform/DirectX12/Render/DirectX12Texture.h"
#endif

namespace VWolf {

    Ref<Texture2D> Texture::LoadTexture2D(uint32_t width, uint32_t height, TextureOptions options) {
        Ref<Texture2D> texture;
        switch(Application::GetApplication()->GetDriverType()) {
            case DriverType::OpenGL:
                texture = CreateRef<OpenGLTexture2D>(width, height, options);
                break;
#ifdef VWOLF_PLATFORM_WINDOWS
            case DriverType::DirectX12:
                texture = CreateRef<DirectX12Texture2D>(width,  height, options);
                break;
#endif
            default:
                VWOLF_CORE_ASSERT(false, "Texture: Not yet implemented");
        }
        return texture;
    }

    Ref<Texture2D> Texture::LoadTexture2D(const std::string filePath, TextureOptions options) {
        Ref<Texture2D> texture;
        switch(Application::GetApplication()->GetDriverType()) {
            case DriverType::OpenGL:
                texture = CreateRef<OpenGLTexture2D>(filePath, options);
                break;
#ifdef VWOLF_PLATFORM_WINDOWS
            case DriverType::DirectX12:
                texture = CreateRef<DirectX12Texture2D>(filePath, options);
                break;
#endif
            default:
                VWOLF_CORE_ASSERT(false, "Texture: Not yet implemented");
        }
        return texture;
    }

    Ref<RenderTexture> Texture::LoadRenderTexture(uint32_t width, uint32_t height, TextureOptions options) {
        Ref<RenderTexture> texture;
        switch(Application::GetApplication()->GetDriverType()) {
            case DriverType::OpenGL:
                texture = CreateRef<OpenGLRenderTexture>(width, height, options);
                break;
#ifdef VWOLF_PLATFORM_WINDOWS
            case DriverType::DirectX12:
                texture = CreateRef<DirectX12RenderTexture>(width, height, options);
                break;
#endif
            default:
                VWOLF_CORE_ASSERT(false, "Render Texture: Not yet implemented");
        }
        return texture;
    }
}
