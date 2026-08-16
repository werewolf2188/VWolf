//
//  MDevice.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 4/22/24.
//
#include "vwpch.h"

#if defined(VWOLF_PLATFORM_MACOS) || defined(VWOLF_PLATFORM_IOS)
#include "MetalDevice.h"
#include "MetalRenderContext.h"
#include "VWolf/Platform/Metal/Core/SmartPointers.h"

namespace VWolf::RHI  {
    Ref<MetalDevice> MetalDevice::GetCurrentDevice() {
        return CreateRef<MetalDevice>(MTL::CreateSystemDefaultDevice());
    }

    MetalDevice::MetalDevice(MTL::Device* device): currentDevice(CreateMetalRef<MTL::Device>(device)) {}

    Ref<RenderContext> MetalDevice::CreateRenderContext(const RenderContextSettings& settings) {
        return CreateRef<MetalRenderContext>(settings, shared_from_this());
    }
}

#endif
