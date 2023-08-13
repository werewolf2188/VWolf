//
//  OpenGLTexture.h
//  VWolf
//
//  Created by Enrique Ricalde on 1/25/23.
//

#pragma once
#include "VWolf/Core/Render/Texture.h"
#include "VWolf/Core/Math/VMath.h"

namespace VWolf {
    class OpenGLBindableTexture {
    public:
        virtual void Bind(uint32_t base) = 0;
        virtual void Unbind(uint32_t base) = 0;
    };

    class OpenGLTexture2D: public Texture2D, public OpenGLBindableTexture {
    public:
        OpenGLTexture2D(TextureDefault textureDefault, uint32_t width, uint32_t height, TextureOptions options = {});
        OpenGLTexture2D(const std::string filePath, TextureOptions options = {});
        virtual ~OpenGLTexture2D();
        virtual void* GetHandler() override;
    public:
        virtual void Bind(uint32_t base) override;
        virtual void Unbind(uint32_t base) override;
    protected:
        virtual void PopulateColor() override;
    private:
#if defined(DEBUG) || defined(_DEBUG)
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

        void Invalidate();
    private:
        GLuint m_colorTextureID = 0;
        GLuint m_depthTextureID = 0;
        GLuint m_frameBufferID = 0;
//        GLenum m_internalDataFormat, m_dataFormat;
    };

    class OpenGLCubemap: public Cubemap, public OpenGLBindableTexture {
    public:
        OpenGLCubemap(TextureDefault textureDefault, uint32_t size, TextureOptions options = {});
        OpenGLCubemap(std::array<std::string, 6> paths, TextureOptions options = {});
        virtual ~OpenGLCubemap();
        virtual void* GetHandler() override;
    public:
        virtual void Bind(uint32_t base) override;
        virtual void Unbind(uint32_t base) override;
    protected:
        void PopulateColor(GLuint id);
        virtual void PopulateColor() override;
    private:
#if defined(DEBUG) || defined(_DEBUG)
        void PopulateTest(GLuint id, int checkIndex, Vector4Float otherColor);
        void PopulateTest();
#endif
    private:
        GLuint m_textureID;
        GLenum m_internalDataFormat, m_dataFormat;
    };
}
