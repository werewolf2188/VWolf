//
//  Sampler.h
//  VWolf
//
//  Created by Enrique Moises on 10/20/26.
//

#pragma once

#include "Types.h"
#include "Resource.h"

namespace VWolf {
    class TextureOptions;
}

namespace VWolf::RHI {
    enum class SamplerMinMag: uint32_t
    {
        Nearest = 0,
        Linear
    };

    enum class SamplerMip: uint32_t
    {
        NotMipmapped = 0,
        Nearest,
        Linear
    };

    class SamplerFilter {
    public:
        SamplerFilter() = default;
        SamplerFilter(SamplerMinMag min, SamplerMinMag mag, SamplerMip mip): min(min), mag(mag), mip(mip) { }
        SamplerFilter(SamplerMinMag min_mag, SamplerMip mip = SamplerMip::NotMipmapped):
        min(min_mag), mag(min_mag), mip(mip) { }
        
        const SamplerMinMag GetMin() const { return min; }
        const SamplerMinMag GetMag() const { return mag; }
        const SamplerMip    GetMip() const { return mip; }
        
        static SamplerFilter FromTextureOptions(const VWolf::TextureOptions& options);
    private:
        SamplerMinMag min = SamplerMinMag::Nearest;
        SamplerMinMag mag = SamplerMinMag::Nearest;
        SamplerMip    mip = SamplerMip::NotMipmapped;
    };

    enum class SamplerMode: uint32_t
    {
        ClampToEdge = 0,
        ClampToZero,
        ClampToBorderColor,
        Repeat,
        RepeatMirror,
    };

    class SamplerAddress {
    public:
        SamplerAddress() = default;
        SamplerAddress(SamplerMode s, SamplerMode t, SamplerMode r): s(s), t(t), r(r) {}
        SamplerAddress(SamplerMode all): s(all), t(all), r(all) {}
        
        const SamplerMode GetS() const { return s; }
        const SamplerMode GetT() const { return t; }
        const SamplerMode GetR() const { return r; }
        
        static SamplerAddress FromTextureOptions(const VWolf::TextureOptions& options);
    private:
        SamplerMode s = SamplerMode::ClampToEdge; // width
        SamplerMode t = SamplerMode::ClampToEdge; // height
        SamplerMode r = SamplerMode::ClampToEdge; // depth
    };

    class SamplerLOD {
    public:
        SamplerLOD() = default;
        SamplerLOD(float bias, float min = 0.F, float max = std::numeric_limits<float>::max()):
        bias(bias), min(min), max(max) { }
        
        const float GetMin () const { return min; }
        const float GetMax () const { return max; }
        const float GetBias () const {return bias; }
    private:
        float min  = 0.F;
        float max  = std::numeric_limits<float>::max();
        float bias = 0.F;
    };

    class SamplerSettings {
    public:
        SamplerSettings() = default;
        SamplerSettings(SamplerFilter filter,
                        SamplerAddress address,
                        SamplerLOD lod,
                        uint32_t max_anisotropy = 1,
                        Compare compare_function = Compare::Never):
        filter(filter), address(address), lod(lod), max_anisotropy(max_anisotropy), compare_function(compare_function) {
        }
        
        const SamplerFilter GetFilter() const { return filter; }
        const SamplerAddress GetAddress() const { return address; }
        const SamplerLOD GetLod() const { return lod; }
        const uint32_t GetMaxAnisotropy() const { return max_anisotropy; }
        const Compare GetCompareFunction() const { return compare_function; }
    private:
        SamplerFilter        filter;
        SamplerAddress       address;
        SamplerLOD           lod;
        uint32_t             max_anisotropy   = 1;
        Compare compare_function = Compare::Never;
    };

    class Sampler: public Resource {
    public:
        Sampler(const SamplerSettings& settings): settings(settings) {}
        virtual ~Sampler() = default;
        
        virtual const ResourceType GetResourceType() const override { return ResourceType::Sampler; }
        
        const SamplerSettings& GetSettings() const { return settings; }
    protected:
        SamplerSettings settings;
    private:
    };

}
