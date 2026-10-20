//
//  MetalSampler.h
//  VWolf
//
//  Created by Enrique Moises on 10/20/26.
//

#pragma once

#include "VWolf/Core/Base.h"
#include "VWolf/Platform/Metal/Core/Core.h"

#include "VWolf/Core/Render/RHI/Sampler.h"

#include "VWolf/Platform/Metal/Render/RHI/MetalRenderContext.h"

namespace VWolf::RHI {
    class MetalSampler: public Sampler, public Shareable<MetalSampler> {
    public:
        MetalSampler(Weak<MetalRenderContext> context, const SamplerSettings& settings);
        ~MetalSampler();
        
        virtual ResourceView GetResourceView() override;
        
        const Ref<MTL::SamplerState> GetSamplerState() { return samplerState; }
    private:
        void Initialize();
        
        Ref<MTL::SamplerState> samplerState;
        Weak<MetalRenderContext> context;
    };
}
