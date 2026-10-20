//
//  OpenGLTexture.cpp
//  VWolf
//
//  Created by Enrique Moises on 10/6/26.
//

#include "vwpch.h"
#include "OpenGLTexture.h"
#include "OpenGLTypes.h"

namespace VWolf::RHI {
    OpenGLTexture::OpenGLTexture(Weak<OpenGLRenderContext> context, const TextureSettings& settings): Texture(settings), context(context) {
        GLThrowIfFailed(glGenTextures(1, &m_textureID));
        Bind();
        
        InitializeTexture();
        
        // TODO: This should come from the setting or the sampler
        GLThrowIfFailed(glTexParameteri(GetTextureType(), GL_TEXTURE_MIN_FILTER, GL_NEAREST));
        GLThrowIfFailed(glTexParameteri(GetTextureType(), GL_TEXTURE_MAG_FILTER, GL_NEAREST));

        GLThrowIfFailed(glTexParameteri(GetTextureType(), GL_TEXTURE_WRAP_S, GL_REPEAT));
        GLThrowIfFailed(glTexParameteri(GetTextureType(), GL_TEXTURE_WRAP_T, GL_REPEAT));
        //
        
        GLThrowIfFailed(glGenerateMipmap(GetTextureType()));
        Unbind();
    }

    void OpenGLTexture::InitializeTexture() {
        switch (settings.GetTextureDimensionType()) {
            case TextureDimensionType::Tex1D:
                GLThrowIfFailed(glTexImage1D(GetTextureType(),
                                             0,
                                             GetInternalFormatFrom(settings.GetFormat()),
                                             settings.GetWidth(),
                                             0,
                                             GetDataFormatFrom(settings.GetFormat()),
                                             GetDataTypeFrom(settings.GetFormat()),
                                             nullptr));
                break;
            case TextureDimensionType::Tex1DArray:
                GLThrowIfFailed(glTexImage3D(GetTextureType(),
                                             0,
                                             GetInternalFormatFrom(settings.GetFormat()),
                                             settings.GetWidth(),
                                             settings.GetArrayLength(),
                                             1,
                                             0,
                                             GetDataFormatFrom(settings.GetFormat()),
                                             GetDataTypeFrom(settings.GetFormat()),
                                             nullptr));
                break;
            case TextureDimensionType::Tex2D:
                GLThrowIfFailed(glTexImage2D(GetTextureType(),
                                             0,
                                             GetInternalFormatFrom(settings.GetFormat()),
                                             settings.GetWidth(),
                                             settings.GetHeight(),
                                             0,
                                             GetDataFormatFrom(settings.GetFormat()),
                                             GetDataTypeFrom(settings.GetFormat()),
                                             nullptr));
                break;
            case TextureDimensionType::Tex2DArray:
                GLThrowIfFailed(glTexImage3D(GetTextureType(),
                                             0,
                                             GetInternalFormatFrom(settings.GetFormat()),
                                             settings.GetWidth(),
                                             settings.GetHeight(),
                                             settings.GetArrayLength(),
                                             0,
                                             GetDataFormatFrom(settings.GetFormat()),
                                             GetDataTypeFrom(settings.GetFormat()),
                                             nullptr));
                break;
            case TextureDimensionType::Tex2DMultisample:
                GLThrowIfFailed(glTexImage2DMultisample(GetTextureType(),
                                             4,
                                             GetInternalFormatFrom(settings.GetFormat()),
                                             settings.GetWidth(),
                                             settings.GetHeight(),
                                             GL_TRUE));
                break;
            case TextureDimensionType::Cube:
                for (int f = 0; f < 6; ++f) {
                    GLThrowIfFailed(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + f,
                                                 0,
                                                 GetInternalFormatFrom(settings.GetFormat()),
                                                 settings.GetWidth(),
                                                 settings.GetHeight(),
                                                 0,
                                                 GetDataFormatFrom(settings.GetFormat()),
                                                 GetDataTypeFrom(settings.GetFormat()),
                                                 nullptr));
                }
                break;
            case TextureDimensionType::CubeArray:
//                GLThrowIfFailed(glTexImage3D(GetTextureType(),
//                                             0,
//                                             GetInternalFormatFrom(settings.GetFormat()),
//                                             settings.GetWidth(),
//                                             settings.GetHeight(),
//                                             settings.GetArrayLength(),
//                                             0,
//                                             GetDataFormatFrom(settings.GetFormat()),
//                                             GetDataTypeFrom(settings.GetFormat()),
//                                             nullptr));
                break;
            case TextureDimensionType::Tex3D:
                GLThrowIfFailed(glTexImage3D(GetTextureType(),
                                             0,
                                             GetInternalFormatFrom(settings.GetFormat()),
                                             settings.GetWidth(),
                                             settings.GetHeight(),
                                             settings.GetDepth(),
                                             0,
                                             GetDataFormatFrom(settings.GetFormat()),
                                             GetDataTypeFrom(settings.GetFormat()),
                                             nullptr));
                break;
        }
    }

    OpenGLTexture::~OpenGLTexture() {
        GLThrowIfFailed(glDeleteTextures(1, &m_textureID));
    }

    const GLuint OpenGLTexture::GetTextureType() const {
        switch (settings.GetTextureDimensionType()) {
            case TextureDimensionType::Tex1D:
                return GL_TEXTURE_1D;
            case TextureDimensionType::Tex1DArray:
                return GL_TEXTURE_1D_ARRAY;
            case TextureDimensionType::Tex2D:
                return GL_TEXTURE_2D;
            case TextureDimensionType::Tex2DArray:
                return GL_TEXTURE_2D_ARRAY;
            case TextureDimensionType::Tex2DMultisample:
                return GL_TEXTURE_2D_MULTISAMPLE;
            case TextureDimensionType::Cube:
                return GL_TEXTURE_CUBE_MAP;
            case TextureDimensionType::CubeArray:
                return GL_TEXTURE_CUBE_MAP_ARRAY;
            case TextureDimensionType::Tex3D:
                return GL_TEXTURE_3D;
        }
    }

    void OpenGLTexture::Bind() {
        GLThrowIfFailed(glBindTexture(GetTextureType(), m_textureID));
    }

    void OpenGLTexture::Unbind() {
        GLThrowIfFailed(glBindTexture(GetTextureType(), 0));
    }

    void OpenGLTexture::Bind(uint32_t base) {
        // TODO: What is this for?
        GLThrowIfFailed(glActiveTexture(GL_TEXTURE0 + base));
        Bind();
    }

    void OpenGLTexture::Unbind(uint32_t base) {
        // TODO: Move everything related to sampler into it's own class
        GLThrowIfFailed(glBindSampler(base, 0));
        //
        Unbind();
    }

    ResourceView OpenGLTexture::GetResourceView() {
        return ResourceView(shared_from_this(), {});
    }

    void OpenGLTexture::SetData(const SubResource& subResource) {
        switch (settings.GetTextureDimensionType()) {
            case TextureDimensionType::Tex1D:
                GLThrowIfFailed(glTexSubImage1D(GetTextureType(),
                                                0,
                                                0,
                                                settings.GetWidth(),
                                                GetDataFormatFrom(settings.GetFormat()),
                                                GetDataTypeFrom(settings.GetFormat()),
                                                subResource.GetBytes()));
                break;
            case TextureDimensionType::Tex1DArray:
                GLThrowIfFailed(glTexSubImage2D(GetTextureType(),
                                                0,
                                                0,
                                                subResource.GetIndex().GetArrayIndex(),
                                                settings.GetWidth(),
                                                1,
                                                GetDataFormatFrom(settings.GetFormat()),
                                                GetDataTypeFrom(settings.GetFormat()),
                                                subResource.GetBytes()));
                break;
            case TextureDimensionType::Tex2D:
                GLThrowIfFailed(glTexSubImage2D(GetTextureType(),
                                                0,
                                                0,
                                                0,
                                                settings.GetWidth(),
                                                settings.GetHeight(),
                                                GetDataFormatFrom(settings.GetFormat()),
                                                GetDataTypeFrom(settings.GetFormat()),
                                                subResource.GetBytes()));
                break;
            case TextureDimensionType::Tex2DArray:
                GLThrowIfFailed(glTexSubImage3D(GetTextureType(),
                                                0,
                                                0,
                                                0,
                                                subResource.GetIndex().GetArrayIndex(),
                                                settings.GetWidth(),
                                                settings.GetHeight(),
                                                1,
                                                GetDataFormatFrom(settings.GetFormat()),
                                                GetDataTypeFrom(settings.GetFormat()),
                                                subResource.GetBytes()));
                break;
            case TextureDimensionType::Tex2DMultisample:
                // NO IDEA WHAT THIS DOES
                break;
            case TextureDimensionType::Cube:
                GLThrowIfFailed(glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + subResource.GetIndex().GetDepthSlice(),
                                                0,
                                                0,
                                                0,
                                                settings.GetWidth(),
                                                settings.GetHeight(),
                                                GetDataFormatFrom(settings.GetFormat()),
                                                GetDataTypeFrom(settings.GetFormat()),
                                                subResource.GetBytes()));
                break;
            case TextureDimensionType::CubeArray:
//                GLThrowIfFailed(glTexSubImage3D(GetTextureType(),
//                                                GL_TEXTURE_CUBE_MAP_POSITIVE_X + subResource.GetIndex().GetDepthSlice(),
//                                                0,
//                                                0,
//                                                subResource.GetIndex().GetArrayIndex(),
//                                                settings.GetWidth(),
//                                                settings.GetHeight(),
//                                                1,
//                                                GetDataFormatFrom(settings.GetFormat()),
//                                                GetDataTypeFrom(settings.GetFormat()),
//                                                subResource.GetBytes()));
                break;
            case TextureDimensionType::Tex3D:
                GLThrowIfFailed(glTexSubImage3D(GetTextureType(),
                                                0,
                                                0,
                                                0,
                                                0,
                                                settings.GetWidth(),
                                                settings.GetHeight(),
                                                settings.GetDepth(),
                                                GetDataFormatFrom(settings.GetFormat()),
                                                GetDataTypeFrom(settings.GetFormat()),
                                                subResource.GetBytes()));
        }
    }

    TextureView OpenGLTexture::GetTextureView(const SubResourceIndex& index) {
        return TextureView(weak_from_this(), index);
    }
}
