//
//  MSurface.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 4/22/24.
//

#include "vwpch.h"

#if defined(VWOLF_PLATFORM_MACOS) || defined(VWOLF_PLATFORM_IOS)
#include "MSurface.h"

namespace VWolf {

    MSurface::MSurface(Ref<MDevice> device, int width, int height) {
        layer = CA::MetalLayer::layer();
        layer->setDevice(device->GetDevice());
        layer->setPixelFormat(GetPixelFormat());
        layer->setFramebufferOnly(true);
        layer->setDrawableSize({static_cast<CGFloat>(width), static_cast<CGFloat>(height)});

        renderPassDescriptor = MTL::RenderPassDescriptor::alloc()->init();
        renderPassDescriptor->depthAttachment()->setClearDepth(1.0f);
        renderPassDescriptor->depthAttachment()->setLoadAction(MTL::LoadAction::LoadActionClear);
        renderPassDescriptor->depthAttachment()->setStoreAction(MTL::StoreAction::StoreActionDontCare);
        MTL::TextureDescriptor* depthBufferDescriptor = MTL::TextureDescriptor::texture2DDescriptor(GetDepthStencilPixelFormat(), width, height, false);
        depthBufferDescriptor->setUsage(MTL::TextureUsageRenderTarget | MTL::TextureUsageShaderRead);
        renderPassDescriptor->depthAttachment()->setTexture(device->GetDevice()->newTexture(depthBufferDescriptor));
        renderPassDescriptor->stencilAttachment()->setTexture(renderPassDescriptor->depthAttachment()->texture());
    }

    MSurface::~MSurface() {
        renderPassDescriptor->release();
        layer->release();
    }

    void MSurface::Begin() {
        drawable = layer->nextDrawable();

        if (drawable == nullptr) {
            VWOLF_CORE_ERROR("Metal driver: Drawable is null");
            throw std::runtime_error("Metal driver: Drawable is null");
        }
    }

    void MSurface::End() {
        
    }
}

#endif
