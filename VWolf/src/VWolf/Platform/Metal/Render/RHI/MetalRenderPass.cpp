//
//  MSurface.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 4/22/24.
//

#include "vwpch.h"

#if defined(VWOLF_PLATFORM_MACOS) || defined(VWOLF_PLATFORM_IOS)
#include "MetalRenderPass.h"
#include "VWolf/Platform/Metal/Core/SmartPointers.h"

#include "MetalDriver.h"
#include "MetalTypes.h"

namespace VWolf::RHI  {

    const MTL::LoadAction GetLoadActionFrom(const RenderPassAttachment::LoadAction action) {
        switch (action) {
            case RenderPassAttachment::LoadAction::DontCare:
                return MTL::LoadAction::LoadActionDontCare;
            case RenderPassAttachment::LoadAction::Load:
                return MTL::LoadAction::LoadActionLoad;
            case RenderPassAttachment::LoadAction::Clear:
                return MTL::LoadAction::LoadActionClear;
        }
    }

    const MTL::StoreAction GetStoreActionFrom(const RenderPassAttachment::StoreAction action) {
        switch (action) {
            case RenderPassAttachment::StoreAction::DontCare:
                return MTL::StoreAction::StoreActionDontCare;
            case RenderPassAttachment::StoreAction::Store:
                return MTL::StoreAction::StoreActionStore;
            case RenderPassAttachment::StoreAction::Resolve:
                return MTL::StoreAction::StoreActionMultisampleResolve;
        }
    }

    MetalRenderPattern::MetalRenderPattern(Weak<MetalRenderContext> context, const RenderPatternSettings& settings):
    RenderPattern(settings), context(context) {
        InitializeDescriptor();
    }

    void MetalRenderPattern::InitializeDescriptor() {
        descriptor = CreateMetalRef<MTL::RenderPassDescriptor>(MTL::RenderPassDescriptor::alloc()->init());
        
        uint32_t index = 0;
        for(const RenderPassColorAttachment& colorAttachment: settings.GetColorAttachments()) {
            descriptor->colorAttachments()->object(index)->setLoadAction(GetLoadActionFrom(colorAttachment.GetLoadAction()));
            descriptor->colorAttachments()->object(index)->setStoreAction(GetStoreActionFrom(colorAttachment.GetStoreAction()));
        }
        
        if (settings.GetDepthAttachment().has_value()) {
            descriptor->depthAttachment()->setClearDepth(1.0f); // 1.0f ????
            const RenderPassDepthAttachment& attachment = settings.GetDepthAttachment().value();
            descriptor->depthAttachment()->setLoadAction(GetLoadActionFrom(attachment.GetLoadAction()));
            descriptor->depthAttachment()->setStoreAction(GetStoreActionFrom(attachment.GetStoreAction()));
        }
        
        if (settings.GetStencilAttachment().has_value()) {
            const RenderPassStencilAttachment& attachment = settings.GetStencilAttachment().value();
            descriptor->stencilAttachment()->setLoadAction(GetLoadActionFrom(attachment.GetLoadAction()));
            descriptor->stencilAttachment()->setStoreAction(GetStoreActionFrom(attachment.GetStoreAction()));
        }
    }

    Ref<RenderPass> MetalRenderPattern::CreateRenderPass(const RenderPassSettings& settings) {
        return CreateRef<MetalRenderPass>(context, *this, settings);
    }

    MetalRenderPass::MetalRenderPass(Weak<MetalRenderContext> context, const RenderPattern& pattern, const RenderPassSettings& settings): RenderPass(pattern, settings), context(context) {
        
        renderPassDescriptor = ((const MetalRenderPattern&)pattern).GetDescriptor();
        
        for(const RHI::TextureView& textureView: settings.GetTextures()) {
            if (Ref<RHI::MetalTexture> texture = DynamicAs<RHI::MetalTexture>(textureView.GetTexture().lock())) {
                switch (texture->GetSettings().GetTextureType()) {
                    case TextureType::Image:
                        break;
                    case TextureType::RenderTarget:
                        renderPassDescriptor->colorAttachments()->object(0)->setTexture(texture->GetTexture().get());
                        break;
                    case TextureType::FrameBuffer:
                        break;
                    case TextureType::DepthStencil:
                        renderPassDescriptor->depthAttachment()->setTexture(texture->GetTexture().get());
                        renderPassDescriptor->stencilAttachment()->setTexture(renderPassDescriptor->depthAttachment()->texture());
                        break;
                    default: break;
                }
            }
        }
    }

    void MetalRenderPass::Resize(uint32_t width, uint32_t height) {
        settings.SetWidth(width);
        settings.SetHeight(height);
        
        ((MetalRenderPattern&)pattern).InitializeDescriptor();
        renderPassDescriptor = ((const MetalRenderPattern&)pattern).GetDescriptor();
        
        for(const RHI::TextureView& textureView: settings.GetTextures()) {
            if (Ref<RHI::MetalTexture> texture = DynamicAs<RHI::MetalTexture>(textureView.GetTexture().lock())) {
                switch (texture->GetSettings().GetTextureType()) {
                    case TextureType::Image:
                        break;
                    case TextureType::RenderTarget:
                        renderPassDescriptor->colorAttachments()->object(0)->setTexture(texture->GetTexture().get());
                        break;
                    case TextureType::FrameBuffer:
                        break;
                    case TextureType::DepthStencil:
                        renderPassDescriptor->depthAttachment()->setTexture(texture->GetTexture().get());
                        renderPassDescriptor->stencilAttachment()->setTexture(renderPassDescriptor->depthAttachment()->texture());
                        break;
                    default: break;
                }
            }
        }
    }

    void MetalRenderPass::Prepare() {
        for(const RHI::TextureView& textureView: settings.GetTextures()) {
            if (Ref<RHI::MetalTexture> texture = DynamicAs<RHI::MetalTexture>(textureView.GetTexture().lock())) {
                switch (texture->GetSettings().GetTextureType()) {
                    case TextureType::FrameBuffer:
                        texture->Prepare();
                        if (texture->GetCurrentDrawable() == nullptr) {
                            VWOLF_CORE_ERROR("Metal driver: Drawable is null");
                            throw std::runtime_error("Metal driver: Drawable is null");
                        }
                        GetRenderPassDescriptor()->colorAttachments()->object(0)->setTexture(texture->GetTexture().get());
                        break;
                    default: break;
                }
            }
        }
    }

}

#endif
