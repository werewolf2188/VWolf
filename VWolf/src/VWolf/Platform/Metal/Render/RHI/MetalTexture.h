//
//  MetalTexture.h
//  VWolf
//
//  Created by Enrique Moises on 8/27/26.
//

#pragma once

#include "VWolf/Core/Base.h"
#include "VWolf/Platform/Metal/Core/Core.h"

#include "VWolf/Core/Render/RHI/Texture.h"

#include "VWolf/Platform/Metal/Render/RHI/MetalRenderContext.h"

namespace VWolf::RHI {
    class MetalTexture: public Texture, public Shareable<MetalTexture> {
    public:
        MetalTexture(Weak<MetalRenderContext> context, const TextureSettings& settings);
        
        virtual ResourceView GetResourceView() override;
        
        virtual void SetData(const SubResource& subResource) override;
        virtual TextureView GetTextureView(const SubResourceIndex& index) override;
        virtual void SetName(const std::string& name) override;
        
        Ref<CA::MetalLayer> GetLayer() { return layer; }
        Ref<CA::MetalDrawable> GetCurrentDrawable() { return drawable; }
        
        Ref<MTL::Texture> GetTexture();
        void Prepare();
        void Update(const TextureSettings& settings);
    protected:
    private:
        
        void Initialize();
        
        Ref<CA::MetalDrawable> drawable;
        Ref<CA::MetalLayer> layer;
        
        Ref<MTL::Texture> texture;
        Ref<MTL::TextureDescriptor> descriptor;
        Weak<MetalRenderContext> context;
    };
}
