//
//  MetalTexture.hpp
//  VWolf
//
//  Created by Enrique Ricalde on 5/11/24.
//

#pragma once

#include "VWolf/Platform/Common/PTexture.h"
#include "VWolf/Core/Math/VMath.h"
#include "VWolf/Platform/Metal/Core/Core.h"

#include "VWolf/Platform/Metal/Render/RHI/MetalRenderPass.h"
#include "VWolf/Platform/Metal/Render/RHI/MetalRenderCommandList.h"
#include "VWolf/Platform/Metal/Render/RHI/MetalTexture.h"
#include "VWolf/Platform/Metal/Render/RHI/MetalSampler.h"

namespace VWolf {
    class MetalTexture2D : public PTexture2D {
    public:
        MetalTexture2D(void * bytes, uint32_t width, uint32_t height, TextureOptions options = {});
        virtual ~MetalTexture2D();
        virtual void* GetHandler() override;
        
        Ref<RHI::MetalTexture> GetRHITexture() { return texture; }
        Ref<RHI::MetalSampler> GetRHISampler() { return sampler; }
    private:
        RHI::TextureSettings settings;
        RHI::SamplerSettings samplerSettings;
        Ref<RHI::MetalTexture> texture;
        Ref<RHI::MetalSampler> sampler;
        bool hasBeenUpload = false;
        size_t numBytes = 0, rowBytes = 0, numRows = 0;
    };

    class MetalRenderTexture : public PRenderTexture {
    public:
        MetalRenderTexture(uint32_t width, uint32_t height, bool isDepthOnly = false, TextureOptions options = {});
        virtual ~MetalRenderTexture();

        virtual void* GetHandler() override;
        virtual void Resize(uint32_t width, uint32_t height) override;
    public:
        void Prepare();
        Ref<MTL::RenderCommandEncoder> StartEncoder();
        void Commit();
    public:
        const Ref<RHI::MetalRenderPass> GetRenderPass() const { return renderPass; }
        const Ref<RHI::MetalRenderCommandList> GetCommandList() const { return commandList; }
        Ref<RHI::MetalTexture> GetRHITexture() { return texture; }
        Ref<RHI::MetalSampler> GetRHISampler() { return sampler; }
        Ref<RHI::MetalTexture> GetRHIDepthTexture() { return depthTexture; }
        Ref<RHI::MetalSampler> GetRHIDepthSampler() { return depthSampler; }
    private:
        void Initialize();
    private:
        RHI::TextureSettings settings;
        RHI::SamplerSettings samplerSettings;
        RHI::TextureSettings depthSettings;
        RHI::SamplerSettings depthSamplerSettings;
        Ref<RHI::MetalTexture> texture;
        Ref<RHI::MetalSampler> sampler;
        Ref<RHI::MetalTexture> depthTexture;
        Ref<RHI::MetalSampler> depthSampler;
        
        RHI::RenderPatternSettings patternSettings;
        RHI::RenderPassSettings passSettings;
        Ref<RHI::MetalRenderPattern> renderPattern;
        Ref<RHI::MetalRenderPass> renderPass;
        Ref<RHI::MetalRenderCommandList> commandList;
        
        bool isDepthOnly = false;
        bool startedEncoding = false;
    };

    class MetalCubemap : public PCubemap {
    public:        
        MetalCubemap(std::array<void *, 6> bytes, uint32_t size, TextureOptions options = {});
        virtual ~MetalCubemap();
        virtual void* GetHandler() override;
        
        Ref<RHI::MetalTexture> GetRHITexture() { return texture; }
        Ref<RHI::MetalSampler> GetRHISampler() { return sampler; }
    private:
        RHI::TextureSettings settings;
        RHI::SamplerSettings samplerSettings;
        Ref<RHI::MetalTexture> texture;
        Ref<RHI::MetalSampler> sampler;
        bool hasBeenUpload = false;
        uint32_t numberOfSides = 6;
    };
}
