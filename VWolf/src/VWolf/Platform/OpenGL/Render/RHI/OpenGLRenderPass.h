//
//  OpenGLRenderPass.h
//  VWolf
//
//  Created by Enrique Moises on 10/5/26.
//

#pragma once

#include "VWolf/Core/Base.h"
#include "VWolf/Platform/OpenGL/Core/GLCore.h"
#include "VWolf/Platform/OpenGL/Core/Bindable.h"

#include "VWolf/Platform/OpenGL/Render/RHI/OpenGLRenderContext.h"

#include "VWolf/Core/Render/RHI/RenderPass.h"

namespace VWolf::RHI {
    class OpenGLRenderPattern: public RenderPattern, public Shareable<OpenGLRenderPattern> {
    public:
        OpenGLRenderPattern(Weak<OpenGLRenderContext> context, const RenderPatternSettings& settings);
        ~OpenGLRenderPattern();
        
        virtual Ref<RenderPass> CreateRenderPass(const RenderPassSettings& settings) override;
    private:
        Weak<OpenGLRenderContext> context;
    };

    class OpenGLRenderPass: public RenderPass, public Shareable<OpenGLRenderPass>, public Bindable {
    public:
        OpenGLRenderPass(Weak<OpenGLRenderContext> context, const RenderPattern& pattern, const RenderPassSettings& settings);
        
        virtual void Resize(uint32_t width, uint32_t height) override;
        virtual void Bind() override;
        virtual void Unbind() override;
        
        const GLuint GetFrameBufferID() const { return m_frameBufferID; }
    private:
        void Initialize();
        
        Weak<OpenGLRenderContext> context;
        
        GLuint m_frameBufferID = 0;
    };
}
