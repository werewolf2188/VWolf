//
//  MetalTexture.cpp
//  VWolf
//
//  Created by Enrique Moises on 8/27/26.
//

#include "vwpch.h"

#if defined(VWOLF_PLATFORM_MACOS) || defined(VWOLF_PLATFORM_IOS)
#include "MetalTexture.h"
#include "MetalTypes.h"
#include "MetalDriver.h"
#include "VWolf/Platform/Metal/Core/SmartPointers.h"

namespace VWolf::RHI {
    MetalTexture::MetalTexture(Weak<MetalRenderContext> context, const TextureSettings& settings): Texture(settings), context(context) {
        Initialize();
    }

    void MetalTexture::Initialize() {
        
        if (settings.GetTextureType() == TextureType::FrameBuffer) {
            
            layer = CreateMetalRef<CA::MetalLayer>(CA::MetalLayer::layer());
            layer->setDevice(context.lock()->GetDevice()->GetDevice().get());
            layer->setPixelFormat(GetFormatFrom(context.lock()->GetSettings().GetColorFormat()));
            layer->setFramebufferOnly(true);
            layer->setDrawableSize({static_cast<CGFloat>(settings.GetWidth()), static_cast<CGFloat>(settings.GetHeight())});
            return;
        }
            
        MTL::PixelFormat format = GetFormatFrom(settings.GetFormat());
        switch (settings.GetTextureDimensionType()) {
            case TextureDimensionType::Tex1D:
                break;
            case TextureDimensionType::Tex1DArray:
                break;
            case TextureDimensionType::Tex2D:
                descriptor = CreateMetalUnownedRef<MTL::TextureDescriptor>(MTL::TextureDescriptor::texture2DDescriptor(format, settings.GetWidth(), settings.GetHeight(), settings.GetMipmapped()));
                break;
            case TextureDimensionType::Tex2DArray:
                break;
            case TextureDimensionType::Tex2DMultisample:
                break;
            case TextureDimensionType::Cube:
                descriptor = CreateMetalUnownedRef<MTL::TextureDescriptor>(MTL::TextureDescriptor::textureCubeDescriptor(format, settings.GetWidth(), settings.GetMipmapped()));
                break;
            case TextureDimensionType::CubeArray:
                break;
            case TextureDimensionType::Tex3D:
                break;
            default:
                descriptor = CreateMetalUnownedRef<MTL::TextureDescriptor>(MTL::TextureDescriptor::texture2DDescriptor(format, settings.GetWidth(), settings.GetHeight(), settings.GetMipmapped()));
        }
        
        descriptor->setUsage(GetTextureUsageFrom(settings.GetResourceUsage()));
        texture = CreateMetalRef<MTL::Texture, MTL::Resource>(MetalDriver::GetCurrent()->GetDevice()->GetDevice()->newTexture(descriptor.get()));
    }

    void MetalTexture::SetData(const SubResource& subResource) {
        MTL::Region region = MTL::Region(0, 0, settings.GetWidth(), settings.GetHeight());
        float bytesNum = 4 * 4; // TODO: Replace
        texture->replaceRegion(region, subResource.GetIndex().GetMipLevel(), subResource.GetIndex().GetDepthSlice(), subResource.GetBytes(), bytesNum * settings.GetWidth(), bytesNum * settings.GetWidth() * settings.GetHeight());
    }

    Ref<MTL::Texture> MetalTexture::GetTexture()  {
        if(drawable) {
            return CreateMetalUnownedRef<MTL::Texture, MTL::Resource>(drawable->texture());
        }
        return texture;
    }

    void MetalTexture::SetName(const std::string& name) {
        Object::SetName(name);
        if (!name.empty()) {
            if (texture) {
                texture->setLabel(NSSTRINGFROM(name.data()));
            }
        }
    }

    ResourceView MetalTexture::GetResourceView() {
        return ResourceView(shared_from_this(), {});
    }

    TextureView MetalTexture::GetTextureView(const SubResourceIndex& index) {
        return TextureView(weak_from_this(), index);
    }

    void MetalTexture::Prepare() {
        drawable = CreateMetalUnownedRef<CA::MetalDrawable, MTL::Drawable>(layer->nextDrawable());
    }

    void MetalTexture::Update(const TextureSettings& settings) {
        this->settings = settings;
        if (settings.GetTextureType() == TextureType::FrameBuffer) {
            VWOLF_CORE_ASSERT(layer != nullptr, "It has not been initialized");
            layer->setDrawableSize({static_cast<CGFloat>(settings.GetWidth()), static_cast<CGFloat>(settings.GetHeight())});
        } else {
            Initialize();
        }
    }
}
#endif
