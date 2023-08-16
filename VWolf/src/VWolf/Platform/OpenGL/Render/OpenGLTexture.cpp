//
//  OpenGLTexture.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 1/25/23.
//

#include "vwpch.h"
#include "OpenGLTexture.h"

#include "stb_image/stb_image.h"

#include "VWolf/Platform/OpenGL/Core/GLCore.h"

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
                default: return GL_NEAREST;
            }
        }
    
        Color Transform(TextureDefault textureDefault) {
            switch(textureDefault) {
                case TextureDefault::White: return Color(1, 1, 1, 1);
                case TextureDefault::Bump: return Color(0.5f, 0.5f, 1, 0.5f);
                case TextureDefault::Black: return Color(0, 0, 0, 1);
                case TextureDefault::Gray: return Color(0.5f, 0.5f, 0.5f, 1);
                case TextureDefault::Red: return Color(1, 0, 0, 1);
            }
        }
    }

    OpenGLTexture2D::OpenGLTexture2D(TextureDefault textureDefault, uint32_t width, uint32_t height, TextureOptions options):
    Texture2D(textureDefault, width, height, options) {
        m_internalDataFormat = GL_RGBA32F;
        m_dataFormat = GL_RGBA;

        GLThrowIfFailed(glGenTextures(1, &m_textureID));
        GLThrowIfFailed(glBindTexture(GL_TEXTURE_2D, m_textureID));
        GLThrowIfFailed(glTexImage2D(GL_TEXTURE_2D, 0, m_internalDataFormat, m_width, m_height, 0, m_dataFormat, GL_FLOAT, NULL));

        GLThrowIfFailed(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, TransformFilterMode(m_options.GetFilterMode())));
        GLThrowIfFailed(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, TransformFilterMode(m_options.GetFilterMode())));

        GLThrowIfFailed(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, TransformWrapMode(m_options.GetWrapMode(),
                                                                                            m_options.GetWrapModeU())));
        GLThrowIfFailed(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, TransformWrapMode(m_options.GetWrapMode(),
                                                                                            m_options.GetWrapModeV())));
        //
        GLThrowIfFailed(glGenerateMipmap(GL_TEXTURE_2D));
        GLThrowIfFailed(glBindTexture(GL_TEXTURE_2D, 0));

        GLThrowIfFailed(glGenSamplers(1, &m_sampleID));
        GLThrowIfFailed(glSamplerParameteri(m_sampleID, GL_TEXTURE_MIN_FILTER, TransformFilterMode(m_options.GetFilterMode())));
        GLThrowIfFailed(glSamplerParameteri(m_sampleID, GL_TEXTURE_MAG_FILTER, TransformFilterMode(m_options.GetFilterMode())));

        GLThrowIfFailed(glSamplerParameteri(m_sampleID, GL_TEXTURE_WRAP_S, TransformWrapMode(m_options.GetWrapMode(),
                                                                                             m_options.GetWrapModeU())));
        GLThrowIfFailed(glSamplerParameteri(m_sampleID, GL_TEXTURE_WRAP_T, TransformWrapMode(m_options.GetWrapMode(),
                                                                                             m_options.GetWrapModeV())));
        PopulateColor();
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
        GLThrowIfFailed(glGenTextures(1, &m_textureID));
        GLThrowIfFailed(glBindTexture(GL_TEXTURE_2D, m_textureID));
        GLThrowIfFailed(glTexImage2D(GL_TEXTURE_2D, 0, m_internalDataFormat, m_width, m_height, 0, m_dataFormat, GL_UNSIGNED_BYTE, img));

        GLThrowIfFailed(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, TransformFilterMode(m_options.GetFilterMode())));
        GLThrowIfFailed(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, TransformFilterMode(m_options.GetFilterMode())));

        GLThrowIfFailed(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, TransformWrapMode(m_options.GetWrapMode(),
                                                                                            m_options.GetWrapModeU())));
        GLThrowIfFailed(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, TransformWrapMode(m_options.GetWrapMode(),
                                                                                            m_options.GetWrapModeV())));
        //
        GLThrowIfFailed(glGenerateMipmap(GL_TEXTURE_2D));
        GLThrowIfFailed(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, m_dataFormat, GL_UNSIGNED_BYTE, img));
        GLThrowIfFailed(glBindTexture(GL_TEXTURE_2D, 0));

        GLThrowIfFailed(glGenSamplers(1, &m_sampleID));
        GLThrowIfFailed(glSamplerParameteri(m_sampleID, GL_TEXTURE_MIN_FILTER, TransformFilterMode(m_options.GetFilterMode())));
        GLThrowIfFailed(glSamplerParameteri(m_sampleID, GL_TEXTURE_MAG_FILTER, TransformFilterMode(m_options.GetFilterMode())));

        GLThrowIfFailed(glSamplerParameteri(m_sampleID, GL_TEXTURE_WRAP_S, TransformWrapMode(m_options.GetWrapMode(),
                                                                                             m_options.GetWrapModeU())));
        GLThrowIfFailed(glSamplerParameteri(m_sampleID, GL_TEXTURE_WRAP_T, TransformWrapMode(m_options.GetWrapMode(),
                                                                                             m_options.GetWrapModeV())));

        
        stbi_image_free(img);
    }

    OpenGLTexture2D::~OpenGLTexture2D() {
        GLThrowIfFailed(glDeleteTextures(1, &m_textureID));
        GLThrowIfFailed(glDeleteSamplers(1, &m_sampleID));
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
        GLThrowIfFailed(glBindTexture(GL_TEXTURE_2D, m_textureID));
        GLThrowIfFailed(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, m_dataFormat, GL_FLOAT, data));
        GLThrowIfFailed(glBindTexture(GL_TEXTURE_2D, 0));
        delete[] data;
    }
#endif

    void OpenGLTexture2D::PopulateColor() {
        size_t size = sizeof(Vector4Float) * m_width * m_height;
        Vector4Float* data = (Vector4Float*)malloc(size);
        memset(data, 0, size);
        uint32_t index = 0;
        Vector4Float value = Transform(m_textureDefault);
        for (uint32_t column = 0; column < m_height; column++) {
            for (uint32_t row = 0; row < m_width; row++) {                
                index = (column * m_height) + row;
                data[index] = value;
            }
        }
        GLThrowIfFailed(glBindTexture(GL_TEXTURE_2D, m_textureID));
        GLThrowIfFailed(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, m_dataFormat, GL_FLOAT, data));
        GLThrowIfFailed(glBindTexture(GL_TEXTURE_2D, 0));
        delete[] data;
    }

    void* OpenGLTexture2D::GetHandler() {
        return (void *)(intptr_t)m_textureID;
    }

    void OpenGLTexture2D::Bind(uint32_t base) {
        GLThrowIfFailed(glActiveTexture(GL_TEXTURE0 + base));
        GLThrowIfFailed(glBindTexture(GL_TEXTURE_2D, m_textureID));
        GLThrowIfFailed(glBindSampler(base, m_sampleID));
    }

    void OpenGLTexture2D::Unbind(uint32_t base) {
        GLThrowIfFailed(glBindSampler(base, 0));
        GLThrowIfFailed(glBindTexture(GL_TEXTURE_2D, 0));
    }

    OpenGLRenderTexture::OpenGLRenderTexture(uint32_t width, uint32_t height, bool isDepthOnly, TextureOptions options):
    RenderTexture(width, height, options), isDepthOnly(isDepthOnly) {
        if (width <= 0 || height <= 0) {
            VWOLF_CORE_ASSERT(false,  "Render texture cannot have a size less or equal than 0");
        }
        Invalidate();
    }

    void OpenGLRenderTexture::Invalidate() {
        if (m_frameBufferID) {
            GLThrowIfFailed(glDeleteFramebuffers(1, &m_frameBufferID));
            if (!isDepthOnly)
                GLThrowIfFailed(glDeleteTextures(1, &m_colorTextureID));
            GLThrowIfFailed(glDeleteTextures(1, &m_depthTextureID));
        }
        GLThrowIfFailed(glGenFramebuffers(1, &m_frameBufferID));
        GLThrowIfFailed(glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferID));

        // TODO: Declare attachments as outside of this block
        // Color attachment
        if (!isDepthOnly) {
            GLThrowIfFailed(glGenTextures(1, &m_colorTextureID));
            GLThrowIfFailed(glBindTexture(GL_TEXTURE_2D, m_colorTextureID));
            
            GLThrowIfFailed(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL));
            
            GLThrowIfFailed(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, TransformFilterMode(m_options.GetFilterMode())));
            GLThrowIfFailed(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, TransformFilterMode(m_options.GetFilterMode())));
            
            GLThrowIfFailed(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, TransformWrapMode(m_options.GetWrapMode(),
                                                                                                m_options.GetWrapModeU())));
            GLThrowIfFailed(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, TransformWrapMode(m_options.GetWrapMode(),
                                                                                                m_options.GetWrapModeV())));
            GLThrowIfFailed(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, TransformWrapMode(m_options.GetWrapMode(),
                                                                                                m_options.GetWrapModeW())));
            
            GLThrowIfFailed(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorTextureID, 0));
            GLThrowIfFailed(glBindTexture(GL_TEXTURE_2D, 0));
        }
        //
        
        // Depth Attachment
        GLThrowIfFailed(glGenTextures(1, &m_depthTextureID));
        GLThrowIfFailed(glBindTexture(GL_TEXTURE_2D, m_depthTextureID));

        if (!isDepthOnly) {
            GLThrowIfFailed(glTexImage2D(
                                         GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_width, m_height, 0,
                                         GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL
                                         ));
        } else {
            GLThrowIfFailed(glTexImage2D(
                                         GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, m_width, m_height, 0,
                                         GL_DEPTH_COMPONENT, GL_FLOAT, NULL
                                         ));
        }

        GLThrowIfFailed(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, TransformFilterMode(m_options.GetFilterMode())));
        GLThrowIfFailed(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, TransformFilterMode(m_options.GetFilterMode())));
        GLThrowIfFailed(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, TransformWrapMode(m_options.GetWrapMode(),
                                                                                            m_options.GetWrapModeW())));
        GLThrowIfFailed(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, TransformWrapMode(m_options.GetWrapMode(),
                                                                                            m_options.GetWrapModeU())));
        GLThrowIfFailed(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, TransformWrapMode(m_options.GetWrapMode(),
                                                                                            m_options.GetWrapModeV())));
    
        if (!isDepthOnly) {
            GLThrowIfFailed(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_depthTextureID, 0));
        } else {
            GLThrowIfFailed(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTextureID, 0));
        }
        GLThrowIfFailed(glBindTexture(GL_TEXTURE_2D, 0));
        //
        
        if (!isDepthOnly) {
            GLThrowIfFailed(glDrawBuffer(GL_COLOR_ATTACHMENT0));
        } else {
            GLThrowIfFailed(glDrawBuffer(GL_NONE));
            GLThrowIfFailed(glReadBuffer(GL_NONE));
        }
        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        switch (status) {
            case GL_FRAMEBUFFER_UNDEFINED:
                VWOLF_CORE_ERROR("Frame buffer undefined error");
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
                VWOLF_CORE_ERROR("Frame buffer incomplete attachment");
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
                VWOLF_CORE_ERROR("Frame buffer incomplete missing attachment");
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
                VWOLF_CORE_ERROR("Frame buffer draw buffer");
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
                VWOLF_CORE_ERROR("Frame buffer read buffer");
                break;
            case GL_FRAMEBUFFER_UNSUPPORTED:
                VWOLF_CORE_ERROR("Frame buffer unsupported");
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
                VWOLF_CORE_ERROR("Frame buffer incomplete multisample");
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
                VWOLF_CORE_ERROR("Frame buffer incomplete layer targets");
                break;
            default:
                break;
        }
        VWOLF_CORE_ASSERT(status == GL_FRAMEBUFFER_COMPLETE, "Render texture complete");
        
        GLThrowIfFailed(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    }

    OpenGLRenderTexture::~OpenGLRenderTexture() {
        GLThrowIfFailed(glDeleteFramebuffers(1, &m_frameBufferID));
        if (!isDepthOnly)
            GLThrowIfFailed(glDeleteTextures(1, &m_colorTextureID));
        GLThrowIfFailed(glDeleteTextures(1, &m_depthTextureID));
    }

    void OpenGLRenderTexture::Bind() {
        GLThrowIfFailed(glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferID));
        GLThrowIfFailed(glViewport(0, 0, m_width, m_height));
    }
    void OpenGLRenderTexture::Unbind() {
        GLThrowIfFailed(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    }

    void OpenGLRenderTexture::ColorTextureBind(uint32_t base) {
        GLThrowIfFailed(glActiveTexture(GL_TEXTURE0 + base));
        GLThrowIfFailed(glBindTexture(GL_TEXTURE_2D, m_colorTextureID));
    }

    void OpenGLRenderTexture::ColorTextureUnbind(uint32_t base) {
        GLThrowIfFailed(glBindTexture(GL_TEXTURE_2D, 0));
    }

    void OpenGLRenderTexture::DepthTextureBind(uint32_t base) {
        GLThrowIfFailed(glActiveTexture(GL_TEXTURE0 + base));
        GLThrowIfFailed(glBindTexture(GL_TEXTURE_2D, m_depthTextureID));
    }

    void OpenGLRenderTexture::DepthTextureUnbind(uint32_t base) {
        GLThrowIfFailed(glBindTexture(GL_TEXTURE_2D, 0));
    }

    void* OpenGLRenderTexture::GetHandler() {
        return (void *)(intptr_t)m_colorTextureID;
    }

    void* OpenGLRenderTexture::GetDepthHandler() {
        return (void *)(intptr_t)m_depthTextureID;
    }

    void OpenGLRenderTexture::Resize(uint32_t width, uint32_t height) {
        if (width == this->m_width && height == this->m_height) return;
        this->m_width = width;
        this->m_height = height;
        Invalidate();
    }

    OpenGLCubemap::OpenGLCubemap(TextureDefault textureDefault, uint32_t size, TextureOptions options): Cubemap(textureDefault, size, options) {
        m_internalDataFormat = GL_RGBA32F;
        m_dataFormat = GL_RGBA;

        GLThrowIfFailed(glGenTextures(1, &m_textureID));
        GLThrowIfFailed(glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureID));
        GLThrowIfFailed(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, TransformFilterMode(m_options.GetFilterMode())));
        GLThrowIfFailed(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, TransformFilterMode(m_options.GetFilterMode())));
        // These are very important to prevent seams
        GLThrowIfFailed(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, TransformWrapMode(m_options.GetWrapMode(),
                                                                                                  m_options.GetWrapModeW())));
        GLThrowIfFailed(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, TransformWrapMode(m_options.GetWrapMode(),
                                                                                                  m_options.GetWrapModeU())));
        GLThrowIfFailed(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, TransformWrapMode(m_options.GetWrapMode(),
                                                                                                  m_options.GetWrapModeV())));

        PopulateColor();
        GLThrowIfFailed(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
        GLThrowIfFailed(glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS));
    }

    OpenGLCubemap::OpenGLCubemap(std::array<std::string, 6> paths, TextureOptions options): Cubemap(paths, options) {
        m_internalDataFormat = GL_RGBA32F;
        m_dataFormat = GL_RGBA;

        GLThrowIfFailed(glGenTextures(1, &m_textureID));
        GLThrowIfFailed(glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureID));
        GLThrowIfFailed(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, TransformFilterMode(m_options.GetFilterMode())));
        GLThrowIfFailed(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, TransformFilterMode(m_options.GetFilterMode())));
        // These are very important to prevent seams
        GLThrowIfFailed(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, TransformWrapMode(m_options.GetWrapMode(),
                                                                                                  m_options.GetWrapModeW())));
        GLThrowIfFailed(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, TransformWrapMode(m_options.GetWrapMode(),
                                                                                                  m_options.GetWrapModeU())));
        GLThrowIfFailed(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, TransformWrapMode(m_options.GetWrapMode(),
                                                                                                  m_options.GetWrapModeV())));

        for (unsigned int i = 0; i < 6; i++)
        {
            int width, height, nrChannels;
            unsigned char* data = stbi_load(paths[i].c_str(), &width, &height, &nrChannels, 0);
            if (nrChannels == 4)
            {
                m_internalDataFormat = GL_RGBA8;
                m_dataFormat = GL_RGBA;
            }
            else if (nrChannels == 3)
            {
                m_internalDataFormat = GL_RGB8;
                m_dataFormat = GL_RGB;
            }
            if (data)
            {
                stbi_set_flip_vertically_on_load(false);
                GLThrowIfFailed(glTexImage2D
                (
                 GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                 0,
                 m_internalDataFormat,
                 width,
                 height,
                 0,
                 m_dataFormat,
                 GL_UNSIGNED_BYTE,
                 data
                 ));
                stbi_image_free(data);
            }
            else
            {
                VWOLF_CORE_ERROR("Failed to load texture: %s", paths[i].c_str());
                stbi_image_free(data);
            }
        }
        GLThrowIfFailed(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
        GLThrowIfFailed(glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS));
    }

    OpenGLCubemap::~OpenGLCubemap() {
        GLThrowIfFailed(glDeleteTextures(1, &m_textureID));
    }

    void* OpenGLCubemap::GetHandler() {
        return (void *)(intptr_t)m_textureID;
    }

    void OpenGLCubemap::Bind(uint32_t base) {
        GLThrowIfFailed(glActiveTexture(GL_TEXTURE0 + base));
        GLThrowIfFailed(glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureID));
    }

    void OpenGLCubemap::Unbind(uint32_t base) {
        GLThrowIfFailed(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
    }

    void OpenGLCubemap::PopulateTest() {
        std::array<Vector4Float, 6> colors = {
            Vector4Float(1, 0, 0, 1),
            Vector4Float(0, 1, 0, 1),
            Vector4Float(0, 0, 1, 1),
            Vector4Float(1, 1, 0, 1),
            Vector4Float(1, 0, 1, 1),
            Vector4Float(0, 1, 1, 1)
        };
        
        std::array<int, 6> indicesToCheck = {
            1,
            2,
            0,
            2,
            1,
            0
        };
        for (unsigned int i = 0; i < 6; i++)
        {
            PopulateTest(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, indicesToCheck[i], colors[i]);
        }
    }

    void OpenGLCubemap::PopulateColor(GLuint id) {
        size_t size = sizeof(Vector4Float) * m_size * m_size;
        Vector4Float* data = (Vector4Float*)malloc(size);
        memset(data, 0, size);
        uint32_t index = 0;
        Vector4Float value = Transform(m_textureDefault);
        for (uint32_t column = 0; column < m_size; column++) {
            for (uint32_t row = 0; row < m_size; row++) {
                index = (column * m_size) + row;
                data[index] = value;
            }
        }
        
        GLThrowIfFailed(glTexImage2D(id, 0, m_internalDataFormat, m_size, m_size, 0, m_dataFormat, GL_FLOAT, data));
        delete[] data;
    }

    void OpenGLCubemap::PopulateColor() {
        for (unsigned int i = 0; i < 6; i++)
        {
            PopulateColor(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
        }
    }

    void OpenGLCubemap::PopulateTest(GLuint id, int checkIndex, Vector4Float otherColor) {
        size_t size = sizeof(Vector4Float) * m_size * m_size;
        Vector4Float* data = (Vector4Float*)malloc(size);
        memset(data, 0, size);
        uint32_t index = 0;
        Vector4Float white(1, 1, 1, 1);
        Vector4Float value = white;
        for (uint32_t column = 0; column < m_size; column++) {
            if (column % 32 == 0) {
                if (value[checkIndex] == 1)
                    value = otherColor;
                else if (value[checkIndex] == 0)
                    value = white;
            }
            for (uint32_t row = 0; row < m_size; row++) {
                if (row % 32 == 0) {
                    if (value[checkIndex] == 1)
                        value = otherColor;
                    else if (value[checkIndex] == 0)
                        value = white;
                }
                index = (column * m_size) + row;
                data[index] = value;
            }
        }
        
        GLThrowIfFailed(glTexImage2D(id, 0, m_internalDataFormat, m_size, m_size, 0, m_dataFormat, GL_FLOAT, data));
        delete[] data;
    }
}
