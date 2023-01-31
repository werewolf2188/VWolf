//
//  OpenGLTexture.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 1/25/23.
//

#include "vwpch.h"
#include "OpenGLTexture.h"

#include "VWolf/Core/Math/Math.h"

#include "stb_image/stb_image.h"

namespace VWolf {

    namespace {
        GLuint TransformWrapMode(TextureWrapMode _wrapMode, TextureWrapMode _wrapModeSide) {
            TextureWrapMode wrapMode = TextureWrapMode::None;
            if (_wrapMode != TextureWrapMode::None)
                wrapMode = _wrapMode;
            else if (_wrapModeSide != TextureWrapMode::None)
                wrapMode = _wrapModeSide;
            if (wrapMode == TextureWrapMode::None) return GL_REPEAT;

            switch (wrapMode) {
                case TextureWrapMode::Repeat: return GL_REPEAT;
                case TextureWrapMode::Clamp: return GL_CLAMP_TO_EDGE;
                case TextureWrapMode::Mirror: return GL_MIRRORED_REPEAT;
                case TextureWrapMode::MirrorOnce: return GL_MIRROR_CLAMP_TO_EDGE;
                default: return GL_REPEAT;
            }
        }

        GLuint TransformFilterMode(TextureFilterMode filterMode) {
            switch (filterMode) {
                case TextureFilterMode::Point: return GL_NEAREST;
                case TextureFilterMode::Bilinear: return GL_LINEAR;
                case TextureFilterMode::Trilinear: return GL_LINEAR;
                default: return GL_REPEAT;
            }
        }
    }

    OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height, TextureOptions options):
    Texture2D(width, height, options) {
        m_internalDataFormat = GL_RGBA32F;
        m_dataFormat = GL_RGBA;

        glGenTextures(1, &m_textureID);
        glBindTexture(GL_TEXTURE_2D, m_textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, m_internalDataFormat, m_width, m_height, 0, m_dataFormat, GL_FLOAT, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, TransformFilterMode(m_options.GetFilterMode()));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, TransformFilterMode(m_options.GetFilterMode()));

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, TransformWrapMode(m_options.GetWrapMode(),
                                                                            m_options.GetWrapModeU()));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, TransformWrapMode(m_options.GetWrapMode(),
                                                                            m_options.GetWrapModeV()));
        //
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);

        glGenSamplers(1, &m_sampleID);
        glSamplerParameteri(m_sampleID, GL_TEXTURE_MIN_FILTER, TransformFilterMode(m_options.GetFilterMode()));
        glSamplerParameteri(m_sampleID, GL_TEXTURE_MAG_FILTER, TransformFilterMode(m_options.GetFilterMode()));

        glSamplerParameteri(m_sampleID, GL_TEXTURE_WRAP_S, TransformWrapMode(m_options.GetWrapMode(),
                                                                             m_options.GetWrapModeU()));
        glSamplerParameteri(m_sampleID, GL_TEXTURE_WRAP_T, TransformWrapMode(m_options.GetWrapMode(),
                                                                             m_options.GetWrapModeV()));
#if defined(DEBUG) || defined(_DEBUG)
        if (m_options.IsTestTexture())
            PopulateTest();
#endif
    }

    OpenGLTexture2D::OpenGLTexture2D(const std::string filePath, TextureOptions options):
    Texture2D(filePath, options) {
        int channels, width, height;
        auto img = stbi_load(filePath.c_str(), &width, &height, &channels, 0);
        m_width = width;
        m_height = height;
        
        if (channels == 4)
        {
            m_internalDataFormat = GL_RGBA8;
            m_dataFormat = GL_RGBA;
        }
        else if (channels == 3)
        {
            m_internalDataFormat = GL_RGB8;
            m_dataFormat = GL_RGB;
        }
        // TODO: Creation should be the same for both constructors. Clean code
        // TODO: Data type not right
        // TODO: Mip maps and resize
        glGenTextures(1, &m_textureID);
        glBindTexture(GL_TEXTURE_2D, m_textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, m_internalDataFormat, m_width, m_height, 0, m_dataFormat, GL_UNSIGNED_BYTE, img);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, TransformFilterMode(m_options.GetFilterMode()));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, TransformFilterMode(m_options.GetFilterMode()));

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, TransformWrapMode(m_options.GetWrapMode(),
                                                                            m_options.GetWrapModeU()));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, TransformWrapMode(m_options.GetWrapMode(),
                                                                            m_options.GetWrapModeV()));
        //
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, m_dataFormat, GL_UNSIGNED_BYTE, img);
        glBindTexture(GL_TEXTURE_2D, 0);

        glGenSamplers(1, &m_sampleID);
        glSamplerParameteri(m_sampleID, GL_TEXTURE_MIN_FILTER, TransformFilterMode(m_options.GetFilterMode()));
        glSamplerParameteri(m_sampleID, GL_TEXTURE_MAG_FILTER, TransformFilterMode(m_options.GetFilterMode()));

        glSamplerParameteri(m_sampleID, GL_TEXTURE_WRAP_S, TransformWrapMode(m_options.GetWrapMode(),
                                                                             m_options.GetWrapModeU()));
        glSamplerParameteri(m_sampleID, GL_TEXTURE_WRAP_T, TransformWrapMode(m_options.GetWrapMode(),
                                                                             m_options.GetWrapModeV()));

        
        stbi_image_free(img);
    }

    OpenGLTexture2D::~OpenGLTexture2D() {
        glDeleteTextures(1, &m_textureID);
        glDeleteSamplers(1, &m_sampleID);
    }
#if defined(DEBUG) || defined(_DEBUG)
    void OpenGLTexture2D::PopulateTest() {
        size_t size = sizeof(Vector4Float) * m_width * m_height;
        Vector4Float* data = (Vector4Float*)malloc(size);
        memset(data, 0, size);
        uint32_t index = 0;
        Vector4Float black(0, 0, 0, 1);
        Vector4Float white(1, 1, 1, 1);
        Vector4Float value = white;
        for (uint32_t column = 0; column < m_height; column++) {
            if (column % 32 == 0) {
                if (value.r == 1)
                    value = black;
                else if (value.r == 0)
                    value = white;
            }
            for (uint32_t row = 0; row < m_width; row++) {
                if (row % 32 == 0) {
                    if (value.r == 1)
                        value = black;
                    else if (value.r == 0)
                        value = white;
                }
                index = (column * m_height) + row;
                data[index] = value;
            }
        }
        glBindTexture(GL_TEXTURE_2D, m_textureID);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, m_dataFormat, GL_FLOAT, data);
        glBindTexture(GL_TEXTURE_2D, 0);
        delete[] data;
    }
#endif
    void* OpenGLTexture2D::GetHandler() {
        return (void *)(intptr_t)m_textureID;
    }

    void OpenGLTexture2D::Bind(uint32_t base) {
        glActiveTexture(GL_TEXTURE0 + base);
        glBindTexture(GL_TEXTURE_2D, m_textureID);
        glBindSampler(base, m_sampleID);
    }

    void OpenGLTexture2D::Unbind(uint32_t base) {
        glBindSampler(base, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    OpenGLRenderTexture::OpenGLRenderTexture(uint32_t width, uint32_t height, TextureOptions options):
    RenderTexture(width, height, options) {
        glGenFramebuffers(1, &m_frameBufferID);
        glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferID);

        // TODO: Invalidate when resize
        // TODO: Declare attachments as outside of this block
        // Color attachment
        glGenTextures(1, &m_colorTextureID);
        glBindTexture(GL_TEXTURE_2D, m_colorTextureID);
          
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, TransformFilterMode(m_options.GetFilterMode()));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, TransformFilterMode(m_options.GetFilterMode()));

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, TransformWrapMode(m_options.GetWrapMode(),
                                                                            m_options.GetWrapModeU()));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, TransformWrapMode(m_options.GetWrapMode(),
                                                                            m_options.GetWrapModeV()));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, TransformWrapMode(m_options.GetWrapMode(),
                                                                            m_options.GetWrapModeW()));

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorTextureID, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        //
        
        // Depth Attachment
        glGenTextures(1, &m_depthTextureID);
        glBindTexture(GL_TEXTURE_2D, m_depthTextureID);

        glTexImage2D(
          GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0,
          GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL
        );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, TransformFilterMode(m_options.GetFilterMode()));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, TransformFilterMode(m_options.GetFilterMode()));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, TransformWrapMode(m_options.GetWrapMode(),
                                                                            m_options.GetWrapModeW()));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, TransformWrapMode(m_options.GetWrapMode(),
                                                                            m_options.GetWrapModeU()));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, TransformWrapMode(m_options.GetWrapMode(),
                                                                            m_options.GetWrapModeV()));
    
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_depthTextureID, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        //
        
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        VWOLF_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Render texture complete");
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0); ;
    }

    OpenGLRenderTexture::~OpenGLRenderTexture() {
        glDeleteFramebuffers(1, &m_frameBufferID);
        glDeleteTextures(1, &m_colorTextureID);
        glDeleteTextures(1, &m_depthTextureID);
    }

    void OpenGLRenderTexture::Bind() {
        glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferID);
        glViewport(0, 0, m_width, m_height);
    }
    void OpenGLRenderTexture::Unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void* OpenGLRenderTexture::GetHandler() {
        return (void *)(intptr_t)m_colorTextureID;
    }

    void OpenGLRenderTexture::Resize(uint32_t width, uint32_t height) {

    }
}
