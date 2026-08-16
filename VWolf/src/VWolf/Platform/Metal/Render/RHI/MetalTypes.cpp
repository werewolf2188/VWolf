//
//  MetalTypes.cpp
//  VWolf
//
//  Created by Enrique Moises on 8/27/26.
//

#include "vwpch.h"

#if defined(VWOLF_PLATFORM_MACOS) || defined(VWOLF_PLATFORM_IOS)
#include "MetalTypes.h"

namespace VWolf::RHI {
    const MTL::PixelFormat GetFormatFrom(const PixelFormat format) {
        switch (format) {
            case PixelFormat::Unknown:
                return MTL::PixelFormat::PixelFormatInvalid;
            case PixelFormat::RGBA8:
                return MTL::PixelFormat::PixelFormatRGBA8Uint;
            case PixelFormat::RGBA8Unorm:
                return MTL::PixelFormat::PixelFormatRGBA8Unorm;
            case PixelFormat::RGBA8Unorm_sRGB:
                return MTL::PixelFormat::PixelFormatRGBA8Unorm_sRGB;
            case PixelFormat::BGRA8Unorm:
                return MTL::PixelFormat::PixelFormatBGRA8Unorm;
            case PixelFormat::BGRA8Unorm_sRGB:
                return MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB;
            case PixelFormat::RGBA32Float:
                return MTL::PixelFormat::PixelFormatRGBA32Float;
            case PixelFormat::R32Float:
                return MTL::PixelFormat::PixelFormatR32Float;
            case PixelFormat::R32Uint:
                return MTL::PixelFormat::PixelFormatR32Uint;
            case PixelFormat::R32Sint:
                return MTL::PixelFormat::PixelFormatR32Sint;
            case PixelFormat::R16Float:
                return MTL::PixelFormat::PixelFormatR16Float;
            case PixelFormat::R16Uint:
                return MTL::PixelFormat::PixelFormatR16Uint;
            case PixelFormat::R16Sint:
                return MTL::PixelFormat::PixelFormatR16Sint;
            case PixelFormat::R16Unorm:
                return MTL::PixelFormat::PixelFormatR16Unorm;
            case PixelFormat::R16Snorm:
                return MTL::PixelFormat::PixelFormatR16Snorm;
            case PixelFormat::R8Uint:
                return MTL::PixelFormat::PixelFormatR8Uint;
            case PixelFormat::R8Sint:
                return MTL::PixelFormat::PixelFormatR8Sint;
            case PixelFormat::R8Unorm:
                return MTL::PixelFormat::PixelFormatR8Unorm;
            case PixelFormat::R8Snorm:
                return MTL::PixelFormat::PixelFormatR8Snorm;
            case PixelFormat::A8Unorm:
                return MTL::PixelFormat::PixelFormatA8Unorm;
            case PixelFormat::Depth32Float:
                return MTL::PixelFormat::PixelFormatDepth32Float;
            case PixelFormat::Depth32Float_Stencil8:
                return MTL::PixelFormat::PixelFormatDepth32Float_Stencil8;
        }
        return MTL::PixelFormat::PixelFormatInvalid;
    }

    const MTL::TextureUsage GetTextureUsageFrom(const ResourceUsage usage) {
        MTL::TextureUsage result = MTL::TextureUsageUnknown;
        
        if ((usage & ResourceUsage::ShaderRead) == ResourceUsage::ShaderRead) {
            result |= MTL::TextureUsageShaderRead;
        }
        if ((usage & ResourceUsage::ShaderWrite) == ResourceUsage::ShaderWrite) {
            result |= MTL::TextureUsageShaderWrite;
        }
        if ((usage & ResourceUsage::RenderTarget) == ResourceUsage::RenderTarget) {
            result |= MTL::TextureUsageRenderTarget;
        }
        
        return result;
    }
}
#endif
