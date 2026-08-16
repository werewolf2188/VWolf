//
//  MSurface.hpp
//  VWolf
//
//  Created by Enrique Ricalde on 4/22/24.
//

#pragma once

#include "VWolf/Core/Base.h"
#include "VWolf/Platform/Metal/Core/Core.h"
#include "VWolf/Platform/Metal/Render/RHI/MetalRenderContext.h"

#include "VWolf/Core/Render/RHI/RenderPass.h"
#include "VWolf/Platform/Metal/Render/RHI/MetalTexture.h"

namespace VWolf::RHI  {

    class MetalRenderPass;

    const MTL::PixelFormat GetFormatFrom(const PixelFormat format);
    const MTL::LoadAction GetLoadActionFrom(const RenderPassAttachment::LoadAction action);
    const MTL::StoreAction GetStoreActionFrom(const RenderPassAttachment::StoreAction action);

    class MetalRenderPattern: public RenderPattern, public Shareable<MetalRenderPattern> {
    public:
        MetalRenderPattern(Weak<MetalRenderContext> context, const RenderPatternSettings& settings);
        
        virtual Ref<RenderPass> CreateRenderPass(const RenderPassSettings& settings) override;
        
        void InitializeDescriptor();
        
        Ref<MTL::RenderPassDescriptor> GetDescriptor() const { return descriptor; }
    protected:
    private:
        Ref<MTL::RenderPassDescriptor> descriptor;
        Weak<MetalRenderContext> context;
    };

    class MetalRenderPass: public RenderPass, public Shareable<MetalRenderPass> {
    public:
        MetalRenderPass(Weak<MetalRenderContext> context, const RenderPattern& pattern, const RenderPassSettings& settings);
    public:
        Ref<MTL::RenderPassDescriptor> GetRenderPassDescriptor() { return renderPassDescriptor; }
        
        void Prepare();
    public:
        virtual void Resize(uint32_t width, uint32_t height) override;
    private:
        Weak<MetalRenderContext> context;
        Ref<MTL::RenderPassDescriptor> renderPassDescriptor;
    };
}
