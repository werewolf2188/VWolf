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
        layer->setPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB);
        layer->setFramebufferOnly(true);
        layer->setDrawableSize({static_cast<CGFloat>(width), static_cast<CGFloat>(height)});

        renderPassDescriptor = MTL::RenderPassDescriptor::alloc()->init();
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
