//
//  OpenGLTexture.h
//  VWolf
//
//  Created by Enrique Moises on 10/6/26.
//

#pragma once

#include "VWolf/Core/Base.h"
#include "VWolf/Platform/OpenGL/Core/GLCore.h"
#include "VWolf/Platform/OpenGL/Core/Bindable.h"

#include "VWolf/Core/Render/RHI/Texture.h"

#include "VWolf/Platform/OpenGL/Render/RHI/OpenGLRenderContext.h"

namespace VWolf::RHI {
    class OpenGLTexture: public Texture, public Shareable<OpenGLTexture>, public Bindable, public BaseBindable {
    public:
        OpenGLTexture(Weak<OpenGLRenderContext> context, const TextureSettings& settings);
        ~OpenGLTexture();
        
        virtual ResourceView GetResourceView() override;
        
        virtual void SetData(const SubResource& subResource) override;
        virtual TextureView GetTextureView(const SubResourceIndex& index) override;
        
        virtual void Bind() override;
        virtual void Unbind() override;
        
        virtual void Bind(uint32_t base) override;
        virtual void Unbind(uint32_t base) override;
        
        const GLuint GetTextureID() const { return m_textureID; }
    private:
        const GLuint GetTextureType() const;
        void InitializeTexture();
    private:
        Weak<OpenGLRenderContext> context;
        GLuint m_textureID;
       
    };
}
