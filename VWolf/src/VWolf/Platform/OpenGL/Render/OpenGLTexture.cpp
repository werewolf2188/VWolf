//
//  OpenGLTexture.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 1/25/23.
//

#include "vwpch.h"
#include "OpenGLTexture.h"

#include "VWolf/Core/Math/Math.h"

namespace VWolf {
    OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height): Texture2D(width, height) {
        m_internalDataFormat = GL_RGBA32F;
        m_dataFormat = GL_RGBA;

        glGenTextures(1, &m_textureID);
        glBindTexture(GL_TEXTURE_2D, m_textureID);
        glTexImage2D(GL_TEXTURE_2D, 1, m_internalDataFormat, m_width, m_height, 0, m_dataFormat, GL_FLOAT, NULL);
        

        // TODO: Externalize
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        //
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);

        glGenSamplers(1, &m_sampleID);
    }

    OpenGLTexture2D::~OpenGLTexture2D() {
        glDeleteTextures(1, &m_textureID);
        glDeleteSamplers(1, &m_sampleID);
    }

    void OpenGLTexture2D::PopulateTest() {
        size_t size = sizeof(Vector4Float) * m_width * m_height;
        Vector4Float* data = (Vector4Float*)malloc(size);
        memset(data, 0, size);
        uint32_t index = 0;
        Vector4Float black(0, 0, 0, 0);
        Vector4Float white(1, 1, 1, 1);
        Vector4Float value = white;
        for (uint32_t column = 0; column < m_height; column++)
            for (uint32_t row = 0; row < m_width; row++) {
                if (column % 32 == 0 || row % 32 == 0) {
                    if (value.r == 1)
                        value = black;
                    else if (value.r == 0)
                        value = white;
                }
                index = (column * m_height) + row;
                data[index] = value;
            }
        glBindTexture(GL_TEXTURE_2D, m_textureID);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, m_dataFormat, GL_UNSIGNED_BYTE, data);
        glBindTexture(GL_TEXTURE_2D, 0);
        delete[] data;
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
}
