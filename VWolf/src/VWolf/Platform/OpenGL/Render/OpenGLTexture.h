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
        OpenGLTexture2D(uint32_t width, uint32_t height);
        virtual ~OpenGLTexture2D();
    public:
        virtual void PopulateTest() override;
    public:
        void Bind(uint32_t base);
        void Unbind(uint32_t base);
    private:
        uint32_t m_textureID;
        uint32_t m_sampleID;
        GLenum m_internalDataFormat, m_dataFormat;
    };
}
