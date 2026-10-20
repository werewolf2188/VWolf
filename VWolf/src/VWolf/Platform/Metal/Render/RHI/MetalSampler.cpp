//
//  MetalSampler.cpp
//  VWolf
//
//  Created by Enrique Moises on 10/20/26.
//

#include "vwpch.h"

#if defined(VWOLF_PLATFORM_MACOS) || defined(VWOLF_PLATFORM_IOS)
#include "MetalSampler.h"
#include "MetalTypes.h"
#include "MetalDriver.h"
#include "VWolf/Platform/Metal/Core/SmartPointers.h"

namespace VWolf::RHI {
    MetalSampler::MetalSampler(Weak<MetalRenderContext> context, const SamplerSettings& settings):
    Sampler(settings), context(context) {
        Initialize();
    }

    MetalSampler::~MetalSampler() {
    }

    ResourceView MetalSampler::GetResourceView() {
        return ResourceView(shared_from_this(), {});
    }

    static const MTL::SamplerMinMagFilter GetFilter(const SamplerMinMag& filter) {
        switch (filter) {
            case SamplerMinMag::Nearest: return MTL::SamplerMinMagFilter::SamplerMinMagFilterNearest;
            case SamplerMinMag::Linear: return MTL::SamplerMinMagFilter::SamplerMinMagFilterLinear;
        }
    }

    static const MTL::SamplerMipFilter GetFilter(const SamplerMip& filter) {
        switch (filter) {
            case SamplerMip::NotMipmapped: return MTL::SamplerMipFilter::SamplerMipFilterNotMipmapped;
            case SamplerMip::Nearest: return MTL::SamplerMipFilter::SamplerMipFilterNearest;
            case SamplerMip::Linear: return MTL::SamplerMipFilter::SamplerMipFilterLinear;
        }
    }

    static const MTL::SamplerAddressMode GetAddressMode(const SamplerMode& mode) {
        switch (mode) {
            case SamplerMode::ClampToEdge: return MTL::SamplerAddressMode::SamplerAddressModeClampToEdge;
            case SamplerMode::ClampToZero: return MTL::SamplerAddressMode::SamplerAddressModeClampToZero;
            case SamplerMode::ClampToBorderColor: return MTL::SamplerAddressMode::SamplerAddressModeClampToBorderColor;
            case SamplerMode::Repeat: return MTL::SamplerAddressMode::SamplerAddressModeRepeat;
            case SamplerMode::RepeatMirror: return MTL::SamplerAddressMode::SamplerAddressModeMirrorRepeat;
        }
    }

    const MTL::CompareFunction GetCompareFunction(const Compare& compare) {
        switch (compare) {
            case Compare::Never: return MTL::CompareFunction::CompareFunctionNever;
            case Compare::Less: return MTL::CompareFunction::CompareFunctionLess;
            case Compare::Equal: return MTL::CompareFunction::CompareFunctionEqual;
            case Compare::LEqual: return MTL::CompareFunction::CompareFunctionLessEqual;
            case Compare::Greater: return MTL::CompareFunction::CompareFunctionGreater;
            case Compare::NotEqual: return MTL::CompareFunction::CompareFunctionNotEqual;
            case Compare::GEqual: return MTL::CompareFunction::CompareFunctionGreaterEqual;
            case Compare::Always: return MTL::CompareFunction::CompareFunctionAlways;
        }
    }

    void MetalSampler::Initialize() {
        Ref<MTL::SamplerDescriptor> descriptor = CreateMetalRef<MTL::SamplerDescriptor>(MTL::SamplerDescriptor::alloc()->init());
        
        descriptor->setMipFilter(GetFilter(settings.GetFilter().GetMip()));
        descriptor->setRAddressMode(GetAddressMode(settings.GetAddress().GetR()));
        descriptor->setTAddressMode(GetAddressMode(settings.GetAddress().GetT()));
        descriptor->setSAddressMode(GetAddressMode(settings.GetAddress().GetS()));
        descriptor->setMaxAnisotropy(settings.GetMaxAnisotropy());
        descriptor->setCompareFunction(GetCompareFunction(settings.GetCompareFunction()));
        descriptor->setLodMinClamp(settings.GetLod().GetMin());
        descriptor->setLodMaxClamp(settings.GetLod().GetMax());
        descriptor->setSupportArgumentBuffers(true);
        
        samplerState = CreateMetalRef<MTL::SamplerState>(MetalDriver::GetCurrent()
                                                         ->GetDevice()->GetDevice()->newSamplerState(descriptor.get()));
    }
}
#endif
