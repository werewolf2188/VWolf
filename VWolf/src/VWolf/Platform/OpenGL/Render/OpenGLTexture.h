//
//  OpenGLTexture.h
//  VWolf
//
//  Created by Enrique Ricalde on 1/25/23.
//

#pragma once
#include "VWolf/Platform/Common/PTexture.h"
#include "VWolf/Core/Math/VMath.h"


#include "VWolf/Platform/OpenGL/Core/Bindable.h"
#include "VWolf/Platform/OpenGL/Render/RHI/OpenGLRenderPass.h"
#include "VWolf/Platform/OpenGL/Render/RHI/OpenGLRenderCommandList.h"
#include "VWolf/Platform/OpenGL/Render/RHI/OpenGLTexture.h"

namespace VWolf {

    class OpenGLTexture2D: public PTexture2D, public BaseBindable {
    public:
        OpenGLTexture2D(void * bytes, uint32_t width, uint32_t height, TextureOptions options = {});
        virtual ~OpenGLTexture2D();
        virtual void* GetHandler() override;
    public:
        virtual void Bind(uint32_t base) override;
        virtual void Unbind(uint32_t base) override;
        
        Ref<RHI::OpenGLTexture> GetRHITexture() { return internalTexture; }
    private:
        Ref<RHI::OpenGLTexture> internalTexture;
    };

    class OpenGLRenderTexture: public PRenderTexture {
    public:
        OpenGLRenderTexture(uint32_t width, uint32_t height, bool isDepthOnly = false, TextureOptions options = {});
        virtual ~OpenGLRenderTexture();

        virtual void* GetHandler() override;

        virtual void Resize(uint32_t width, uint32_t height) override;
        
        void Prepare();
        void Commit();

        void ColorTextureBind(uint32_t base);
        void ColorTextureUnbind(uint32_t base);
        void DepthTextureBind(uint32_t base);
        void DepthTextureUnbind(uint32_t base);
        void Bind();
        void Unbind();
        void* GetDepthHandler();
        
        void Invalidate();
        
        Ref<RHI::OpenGLTexture> GetRHITexture() { return colorTexture; }
        Ref<RHI::OpenGLTexture> GetRHIDepthTexture() { return depthTexture; }
        
        const Ref<RHI::OpenGLRenderCommandList> GetCommandList();
    private:
        
        Ref<RHI::OpenGLTexture> colorTexture;
        Ref<RHI::OpenGLTexture> depthTexture;
        
        RHI::RenderPatternSettings patternSettings;
        RHI::RenderPassSettings passSettings;
        Ref<RHI::OpenGLRenderPattern> renderPattern;
        Ref<RHI::OpenGLRenderPass> renderPass;
        Ref<RHI::OpenGLRenderCommandList> commandList;

        bool isDepthOnly = false;
    };

    class OpenGLCubemap: public PCubemap, public BaseBindable {
    public:        
        OpenGLCubemap(std::array<void *, 6> bytes, uint32_t size, TextureOptions options);
        virtual ~OpenGLCubemap();
        virtual void* GetHandler() override;
    public:
        virtual void Bind(uint32_t base) override;
        virtual void Unbind(uint32_t base) override;
        
        Ref<RHI::OpenGLTexture> GetRHITexture() { return internalTexture; }
    private:
        Ref<RHI::OpenGLTexture> internalTexture;
    };
}
