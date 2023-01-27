//
//  OpenGLTexture.h
//  VWolf
//
//  Created by Enrique Ricalde on 1/25/23.
//

#pragma once
#include "VWolf/Core/Render/Texture.h"

namespace VWolf {
    class OpenGLTexture2D: public Texture2D {
    public:
        OpenGLTexture2D(uint32_t width, uint32_t height, TextureOptions options = {});
        OpenGLTexture2D(const std::string filePath, TextureOptions options = {});
        virtual ~OpenGLTexture2D();
        virtual void* GetHandler() override;
    public:
        void Bind(uint32_t base);
        void Unbind(uint32_t base);
#if defined(DEBUG) || defined(_DEBUG)
    private:
        void PopulateTest();
#endif
    private:
        GLuint m_textureID;
        GLuint m_sampleID;
        GLenum m_internalDataFormat, m_dataFormat;
    };

    class OpenGLRenderTexture: public RenderTexture {
    public:
        OpenGLRenderTexture(uint32_t width, uint32_t height, TextureOptions options = {});
        virtual ~OpenGLRenderTexture();

        virtual void* GetHandler() override;

        virtual void Resize(uint32_t width, uint32_t height) override;
    public:
        void Bind();
        void Unbind();
    private:
        GLuint m_colorTextureID;
        GLuint m_depthTextureID;
        GLuint m_frameBufferID;
//        GLenum m_internalDataFormat, m_dataFormat;
    };
}
