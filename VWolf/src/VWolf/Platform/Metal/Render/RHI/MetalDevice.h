//
//  MDevice.hpp
//  VWolf
//
//  Created by Enrique Ricalde on 4/22/24.
//

#pragma once

#include "VWolf/Core/Base.h"
#include "VWolf/Core/Render/RHI/Device.h"
#include "VWolf/Core/Render/RHI/Context.h"
#include "VWolf/Platform/Metal/Core/Core.h"

namespace VWolf::RHI  {
    class MetalDevice: public Device, public Shareable<MetalDevice> {
    public:
        static Ref<MetalDevice> GetCurrentDevice();
    public:
        MetalDevice(MTL::Device* device);
        ~MetalDevice() = default;
    public:
        inline Ref<MTL::Device> GetDevice() { return currentDevice; }
        
        virtual Ref<RenderContext> CreateRenderContext(const RenderContextSettings& settings) override;
    private:
        Ref<MTL::Device> currentDevice;
    };
}
