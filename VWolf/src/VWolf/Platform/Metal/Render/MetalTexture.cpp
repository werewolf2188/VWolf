//
//  MetalTexture.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 5/11/24.
//

#include "vwpch.h"

#if defined(VWOLF_PLATFORM_MACOS) || defined(VWOLF_PLATFORM_IOS)
#include "MetalTexture.h"

#include "VWolf/Platform/Metal/Render/RHI/MetalDriver.h"

namespace VWolf {

    static RHI::SamplerSettings CreateFrom(const TextureOptions& options) {
        RHI::SamplerFilter filter = RHI::SamplerFilter::FromTextureOptions(options);
        RHI::SamplerAddress address = RHI::SamplerAddress::FromTextureOptions(options);
        RHI::SamplerLOD lod;
        
        return { filter, address, lod };
    }

    MetalTexture2D::MetalTexture2D(void * bytes, uint32_t width, uint32_t height, TextureOptions options):
    PTexture2D(bytes, width, height, options) {
        settings = RHI::TextureSettings::ForImage(RHI::PixelFormat::RGBA32Float, width, height);
        texture = DynamicAs<RHI::MetalTexture>(MetalDriver::GetCurrent()->GetRenderContext()->CreateTexture(settings));
        RHI::SubResource subResource(bytes, {});
        texture->SetData(subResource);
        samplerSettings = CreateFrom(options);
        sampler = DynamicAs<RHI::MetalSampler>(MetalDriver::GetCurrent()->GetRenderContext()->CreateSampler(samplerSettings));
    }

    MetalTexture2D::~MetalTexture2D() {
        if (m_bytes != nullptr) {
            free(m_bytes);
        }
    }

    void* MetalTexture2D::GetHandler() {
        return texture->GetTexture().get();
    }

    MetalRenderTexture::MetalRenderTexture(uint32_t width, uint32_t height, bool isDepthOnly, TextureOptions options): PRenderTexture(width, height, options), isDepthOnly(isDepthOnly) {
        Initialize();
    }
    
    MetalRenderTexture::~MetalRenderTexture() {
    }

    void* MetalRenderTexture::GetHandler() {
        return isDepthOnly ? depthTexture->GetTexture().get(): texture->GetTexture().get();
    }

    void MetalRenderTexture::Resize(uint32_t width, uint32_t height) {
        // TODO: What to do with this?
        if (m_width == width && m_height == height) return;
        
        m_width = width;
        m_height = height;
        Initialize();
    }

    void MetalRenderTexture::Initialize() {

        settings = RHI::TextureSettings::ForFrameBuffer(RHI::PixelFormat::RGBA8Unorm_sRGB, m_width, m_height);
        texture = DynamicAs<RHI::MetalTexture>(MetalDriver::GetCurrent()->GetRenderContext()->CreateTexture(settings));
        depthSettings = RHI::TextureSettings::ForFrameBuffer(RHI::PixelFormat::Depth32Float_Stencil8, m_width, m_height);
        depthTexture = DynamicAs<RHI::MetalTexture>(MetalDriver::GetCurrent()->GetRenderContext()->CreateTexture(depthSettings));

        patternSettings = RHI::RenderPatternSettings(
                        {
                            RHI::RenderPassColorAttachment(RHI::PixelFormat::RGBA8Unorm_sRGB,
                                                      RHI::RenderPassAttachment::LoadAction::Clear)
                        },
                        RHI::RenderPassDepthAttachment(RHI::PixelFormat::Depth32Float_Stencil8,
                                                  RHI::RenderPassAttachment::LoadAction::Clear,
                                                  isDepthOnly ? RHI::RenderPassAttachment::StoreAction::Store : RHI::RenderPassAttachment::StoreAction::DontCare),
                        std::optional<RHI::RenderPassStencilAttachment>()
                        );
        
        passSettings = RHI::RenderPassSettings({ texture->GetTextureView({}), depthTexture->GetTextureView({})}, m_width, m_height);
        
        renderPattern = DynamicAs<RHI::MetalRenderPattern>(MetalDriver::GetCurrent()->GetRenderContext()->CreateRenderPattern(patternSettings));
        renderPass = DynamicAs<RHI::MetalRenderPass>(renderPattern->CreateRenderPass(passSettings));
        
        renderPass->GetRenderPassDescriptor()->colorAttachments()->object(0)->setTexture(texture->GetTexture().get());
        renderPass->GetRenderPassDescriptor()->depthAttachment()->setTexture(depthTexture->GetTexture().get());
        renderPass->GetRenderPassDescriptor()->stencilAttachment()->setTexture(depthTexture->GetTexture().get());
        
        sampler = DynamicAs<RHI::MetalSampler>(MetalDriver::GetCurrent()->GetRenderContext()->CreateSampler(samplerSettings));
        depthSampler = DynamicAs<RHI::MetalSampler>(MetalDriver::GetCurrent()->GetRenderContext()->CreateSampler(depthSamplerSettings));
    }

    void MetalRenderTexture::Prepare() {
        commandList = DynamicAs<RHI::MetalRenderCommandList>(MetalDriver::GetCurrent()->GetCommandQueue()
                                                             ->CreateRenderCommandList(renderPass->weak_from_this()));
    }

    Ref<MTL::RenderCommandEncoder> MetalRenderTexture::StartEncoder() {
        return commandList->GetEncoder();
    }

    void MetalRenderTexture::Commit() {
        commandList->Commit();
    }

    MetalCubemap::MetalCubemap(std::array<void *, 6> bytes, uint32_t size, TextureOptions options): PCubemap(bytes, size, options) {
        settings = RHI::TextureSettings::ForCubeImage(RHI::PixelFormat::RGBA32Float, size);
        texture = DynamicAs<RHI::MetalTexture>(MetalDriver::GetCurrent()->GetRenderContext()->CreateTexture(settings));
        for (int i = 0; i < bytes.size(); i++) {
            RHI::SubResource subResource(bytes[i], { static_cast<uint32_t>(i), 0, 0 });
            texture->SetData(subResource);
        }
        samplerSettings = CreateFrom(options);
        sampler = DynamicAs<RHI::MetalSampler>(MetalDriver::GetCurrent()->GetRenderContext()->CreateSampler(samplerSettings));
    }

    MetalCubemap::~MetalCubemap() {
        for (void* data: m_bytes) {
            if (data)
                free(data);
        }
    }

    void* MetalCubemap::GetHandler() {
        return texture->GetTexture().get();
    }
}

#endif
