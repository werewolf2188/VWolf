//
//  MSurface.hpp
//  VWolf
//
//  Created by Enrique Ricalde on 4/22/24.
//

#pragma once

#include "VWolf/Core/Base.h"
#include "Core.h"
#include "MDevice.h"

namespace VWolf {
    class MSurfaceDelegate;

    class MSurface {
    public:
        MSurface(Ref<MDevice> device, int width, int height);
        ~MSurface();
    public:
        CA::MetalLayer* GetLayer() { return layer; }
        CA::MetalDrawable* GetCurrentDrawable() { return drawable; }
        MTL::RenderPassDescriptor* GetRenderPassDescriptor() { return renderPassDescriptor; }
    public:
        MTL::PixelFormat GetPixelFormat() { return MTL::PixelFormat::PixelFormatRGBA8Unorm_sRGB; }
        MTL::PixelFormat GetDepthStencilPixelFormat() { return MTL::PixelFormat::PixelFormatDepth32Float_Stencil8; }
    public:
        void Begin();
        void End();
    private:
        CA::MetalDrawable* drawable;
        CA::MetalLayer *layer;
        MTL::RenderPassDescriptor* renderPassDescriptor;
    };
}
