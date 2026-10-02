//
//  OpenGLRenderPass.cpp
//  VWolf
//
//  Created by Enrique Moises on 10/5/26.
//

#include "vwpch.h"
#include "OpenGLRenderPass.h"
#include "OpenGLTexture.h"

namespace VWolf::RHI {
    OpenGLRenderPattern::OpenGLRenderPattern(Weak<OpenGLRenderContext> context, const RenderPatternSettings& settings):
    RenderPattern(settings), context(context) {

    }

    OpenGLRenderPattern::~OpenGLRenderPattern() {
        
    }

    Ref<RenderPass> OpenGLRenderPattern::CreateRenderPass(const RenderPassSettings& settings) {
        return CreateRef<OpenGLRenderPass>(context, *this, settings);
    }

    OpenGLRenderPass::OpenGLRenderPass(Weak<OpenGLRenderContext> context, const RenderPattern& pattern, const RenderPassSettings& settings):
    RenderPass(pattern, settings), context(context) {
        GLThrowIfFailed(glGenFramebuffers(1, &m_frameBufferID));
        Bind();
        Initialize();
        Unbind();
    }

    void OpenGLRenderPass::Bind() {
        GLThrowIfFailed(glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferID));
    }

    void OpenGLRenderPass::Unbind() {
        GLThrowIfFailed(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    }

    void OpenGLRenderPass::Initialize() {
        
        for(const RHI::TextureView& textureView: settings.GetTextures()) {
            if (Ref<RHI::OpenGLTexture> texture = DynamicAs<RHI::OpenGLTexture>(textureView.GetTexture().lock())) {
                switch (texture->GetSettings().GetTextureType()) {
                    case TextureType::Image:
                        break;
                    case TextureType::RenderTarget:
                        GLThrowIfFailed(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->GetTextureID(), 0));
                        break;
                    case TextureType::FrameBuffer:
                        break;
                    case TextureType::DepthStencil:
                        GLThrowIfFailed(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texture->GetTextureID(), 0));
                        break;
                    default: break;
                }
            }
        }
        
        GLThrowIfFailed(glDrawBuffer(GL_COLOR_ATTACHMENT0));
        
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
    }

    void OpenGLRenderPass::Resize(uint32_t width, uint32_t height) {
        GLThrowIfFailed(glViewport(0, 0, width, height));
    }
}
