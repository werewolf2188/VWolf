//
//  OpenGLTypes.cpp
//  VWolf
//
//  Created by Enrique Moises on 10/5/26.
//

#include "vwpch.h"
#include "OpenGLTypes.h"

namespace VWolf::RHI {
    const GLuint GetInternalFormatFrom(const PixelFormat format) {
        switch (format) {
            case PixelFormat::Unknown:
                return GL_INVALID_VALUE;
            case PixelFormat::RGBA8:
            case PixelFormat::BGRA8Unorm:
            case PixelFormat::RGBA8Unorm:
                return GL_RGBA8;
            case PixelFormat::RGBA8Unorm_sRGB:
                return GL_SRGB8_ALPHA8;
            case PixelFormat::BGRA8Unorm_sRGB:
                return GL_SRGB8_ALPHA8;
            case PixelFormat::RGBA32Float:
                return GL_RGBA32F;
            case PixelFormat::R32Float:
                return GL_R32F;
            case PixelFormat::R32Uint:
                return GL_R32UI;
            case PixelFormat::R32Sint:
                return GL_R32I;
            case PixelFormat::R16Float:
                return GL_R16F;
            case PixelFormat::R16Uint:
                return GL_R16UI;
            case PixelFormat::R16Sint:
                return GL_R16I;
            case PixelFormat::R16Unorm:
                return GL_INVALID_VALUE;
            case PixelFormat::R16Snorm:
                return GL_R16_SNORM;
            case PixelFormat::R8Uint:
                return GL_R8UI;
            case PixelFormat::R8Sint:
                return GL_R8I;
            case PixelFormat::R8Unorm:
                return GL_R16;
            case PixelFormat::R8Snorm:
                return GL_R8_SNORM;
            case PixelFormat::A8Unorm:
                return GL_R8;
            case PixelFormat::Depth32Float:
                return GL_DEPTH_COMPONENT32F;
            case PixelFormat::Depth32Float_Stencil8:
                return GL_DEPTH32F_STENCIL8;
        }
        return GL_INVALID_VALUE;
    }

    const uint32_t GetDataFormatFrom(const PixelFormat format) {
        switch (format) {
            case PixelFormat::Unknown:
                return GL_INVALID_VALUE;
            case PixelFormat::R32Float:
            case PixelFormat::R32Uint:
            case PixelFormat::R32Sint:
            case PixelFormat::R16Float:
            case PixelFormat::R16Uint:
            case PixelFormat::R16Sint:
            case PixelFormat::R16Unorm:
            case PixelFormat::R16Snorm:
            case PixelFormat::R8Uint:
            case PixelFormat::R8Sint:
            case PixelFormat::R8Unorm:
            case PixelFormat::R8Snorm:
            case PixelFormat::A8Unorm:
                return GL_RED;
            case PixelFormat::RGBA8:
            case PixelFormat::BGRA8Unorm:
            case PixelFormat::RGBA8Unorm:
            case PixelFormat::RGBA32Float:
                return GL_RGBA;
            case PixelFormat::RGBA8Unorm_sRGB:
            case PixelFormat::BGRA8Unorm_sRGB:
                return GL_SRGB8_ALPHA8;
            case PixelFormat::Depth32Float:
                return GL_DEPTH_COMPONENT;
            case PixelFormat::Depth32Float_Stencil8:
                return GL_DEPTH_STENCIL;
        }
        return GL_INVALID_VALUE;
    }

    const uint32_t GetDataTypeFrom(const PixelFormat format) {
        switch (format) {
            case PixelFormat::Unknown:
                return GL_INVALID_VALUE;
            case PixelFormat::R32Float:
            case PixelFormat::R16Float:
            case PixelFormat::RGBA32Float:
            case PixelFormat::Depth32Float:
                return GL_FLOAT;
            case PixelFormat::R32Uint:
                return GL_UNSIGNED_INT;
            case PixelFormat::R16Uint:
            case PixelFormat::R16Unorm:
                return GL_UNSIGNED_SHORT;
            case PixelFormat::R8Uint:
            case PixelFormat::R8Unorm:
            case PixelFormat::A8Unorm:
            case PixelFormat::RGBA8Unorm:
            case PixelFormat::BGRA8Unorm:
                return GL_UNSIGNED_BYTE;
            case PixelFormat::R32Sint:
                return GL_INT;
            case PixelFormat::R16Sint:
            case PixelFormat::R16Snorm:
                return GL_SHORT;
            case PixelFormat::RGBA8:
            case PixelFormat::R8Snorm:
            case PixelFormat::R8Sint:
            case PixelFormat::RGBA8Unorm_sRGB:
            case PixelFormat::BGRA8Unorm_sRGB:
                return GL_BYTE;
            case PixelFormat::Depth32Float_Stencil8:
                return GL_FLOAT_32_UNSIGNED_INT_24_8_REV;
        }
        return GL_INVALID_VALUE;
    }
}
