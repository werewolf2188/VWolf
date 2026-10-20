//
//  OpenGLTexture.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 1/25/23.
//

#include "vwpch.h"
#include "OpenGLTexture.h"

#include "stb_image/stb_image.h"

#include "VWolf/Platform/OpenGL/Core/GLCore.h"
#include "VWolf/Platform/OpenGL/Render/RHI/OpenGLDriver.h"

namespace VWolf {

    static RHI::SamplerSettings CreateFrom(const TextureOptions& options) {
        RHI::SamplerFilter filter = RHI::SamplerFilter::FromTextureOptions(options);
        RHI::SamplerAddress address = RHI::SamplerAddress::FromTextureOptions(options);
        RHI::SamplerLOD lod;
        
        return { filter, address, lod };
    }

    OpenGLTexture2D::OpenGLTexture2D(void * bytes, uint32_t width, uint32_t height, TextureOptions options): PTexture2D(bytes, width, height, options) {
        RHI::TextureSettings settings = RHI::TextureSettings::ForImage(RHI::PixelFormat::RGBA32Float, width, height);
        internalTexture = DynamicAs<RHI::OpenGLTexture>(OpenGLDriver::GetCurrentDriver()->GetRenderContext()->CreateTexture(settings));
        RHI::SubResource subResource(bytes, {});
        internalTexture->Bind();
        internalTexture->SetData(subResource);
        internalTexture->Unbind();
        
        RHI::SamplerSettings samplerSettings = CreateFrom(options);
        internalSampler = DynamicAs<RHI::OpenGLSampler>(OpenGLDriver::GetCurrentDriver()->GetRenderContext()->CreateSampler(samplerSettings));
    }

    OpenGLTexture2D::~OpenGLTexture2D() {
        if (m_bytes)
            free(m_bytes);
    }

    void* OpenGLTexture2D::GetHandler() {
        return (void *)(intptr_t)internalTexture->GetTextureID();
    }

    void OpenGLTexture2D::Bind(uint32_t base) {
        internalTexture->Bind(base);
    }

    void OpenGLTexture2D::Unbind(uint32_t base) {
        internalTexture->Unbind(base);
    }

    OpenGLRenderTexture::OpenGLRenderTexture(uint32_t width, uint32_t height, bool isDepthOnly, TextureOptions options):
    PRenderTexture(width, height, options), isDepthOnly(isDepthOnly) {
        if (width <= 0 || height <= 0) {
            VWOLF_CORE_ASSERT(false,  "Render texture cannot have a size less or equal than 0");
        }
        Invalidate();
    }

    void OpenGLRenderTexture::Invalidate() {
        colorTexture = nullptr;
        depthTexture = nullptr;
        
        RHI::PixelFormat colorFormat = RHI::PixelFormat::RGBA8;
        RHI::PixelFormat depthormat = RHI::PixelFormat::Depth32Float_Stencil8;
        
        patternSettings = RHI::RenderPatternSettings(
                        {
                            RHI::RenderPassColorAttachment(colorFormat,
                                                      RHI::RenderPassAttachment::LoadAction::Clear)
                        },
                        RHI::RenderPassDepthAttachment(depthormat,
                                                  RHI::RenderPassAttachment::LoadAction::Clear,
                                                  isDepthOnly ? RHI::RenderPassAttachment::StoreAction::Store : RHI::RenderPassAttachment::StoreAction::DontCare),
                        std::optional<RHI::RenderPassStencilAttachment>()
                        );
        
        renderPattern = DynamicAs<RHI::OpenGLRenderPattern>(OpenGLDriver::GetCurrentDriver()->GetRenderContext()->CreateRenderPattern(patternSettings));
        
        // Color attachment
        RHI::TextureSettings colorSettings = RHI::TextureSettings::ForFrameBuffer(colorFormat, m_width, m_height);
        colorTexture = DynamicAs<RHI::OpenGLTexture>(OpenGLDriver::GetCurrentDriver()->GetRenderContext()->CreateTexture(colorSettings));
        RHI::SamplerSettings colorSamplerSettings = CreateFrom(m_options);
        internalColorSampler = DynamicAs<RHI::OpenGLSampler>(OpenGLDriver::GetCurrentDriver()->GetRenderContext()->CreateSampler(colorSamplerSettings));
        
        // Depth Attachment
        RHI::TextureSettings depthSettingsSettings = RHI::TextureSettings::ForDepthStencil(depthormat, m_width, m_height);
        depthTexture = DynamicAs<RHI::OpenGLTexture>(OpenGLDriver::GetCurrentDriver()->GetRenderContext()->CreateTexture(depthSettingsSettings));
        RHI::SamplerSettings depthSamplerSettings = CreateFrom(m_options);
        internalDepthSampler = DynamicAs<RHI::OpenGLSampler>(OpenGLDriver::GetCurrentDriver()->GetRenderContext()->CreateSampler(depthSamplerSettings));
        
        passSettings = RHI::RenderPassSettings({ colorTexture->GetTextureView({}), depthTexture->GetTextureView({})}, m_width, m_height);
        renderPass = DynamicAs<RHI::OpenGLRenderPass>(renderPattern->CreateRenderPass(passSettings));
    }

    OpenGLRenderTexture::~OpenGLRenderTexture() {
    }

    void OpenGLRenderTexture::Prepare() {
        commandList = DynamicAs<RHI::OpenGLRenderCommandList>(OpenGLDriver::GetCurrentDriver()->GetCommandQueue()
                                                             ->CreateRenderCommandList(renderPass->weak_from_this()));
    }

    const Ref<RHI::OpenGLRenderCommandList> OpenGLRenderTexture::GetCommandList() {
        Bind();
        return commandList;
    }

    void OpenGLRenderTexture::Commit() {
        commandList->Commit();
        Unbind();
    }

    void OpenGLRenderTexture::Bind() {
        renderPass->Bind();
        renderPass->Resize(m_width, m_height);
    }
    void OpenGLRenderTexture::Unbind() {
        renderPass->Unbind();
    }

    void OpenGLRenderTexture::ColorTextureBind(uint32_t base) {
        colorTexture->Bind(base);
    }

    void OpenGLRenderTexture::ColorTextureUnbind(uint32_t base) {
        colorTexture->Unbind(base);
    }

    void OpenGLRenderTexture::DepthTextureBind(uint32_t base) {
        depthTexture->Bind(base);
    }

    void OpenGLRenderTexture::DepthTextureUnbind(uint32_t base) {
        depthTexture->Unbind(base);
    }

    void* OpenGLRenderTexture::GetHandler() {
        return (void *)(intptr_t)colorTexture->GetTextureID();
    }

    void* OpenGLRenderTexture::GetDepthHandler() {
        return (void *)(intptr_t)depthTexture->GetTextureID();
    }

    void OpenGLRenderTexture::Resize(uint32_t width, uint32_t height) {
        if (width == this->m_width && height == this->m_height) return;
        this->m_width = width;
        this->m_height = height;
        
        Invalidate();
    }

    OpenGLCubemap::OpenGLCubemap(std::array<void *, 6> bytes, uint32_t size, TextureOptions options): PCubemap(bytes, size, options) {
        RHI::TextureSettings settings = RHI::TextureSettings::ForCubeImage(RHI::PixelFormat::RGBA32Float, size);
        internalTexture = DynamicAs<RHI::OpenGLTexture>(OpenGLDriver::GetCurrentDriver()->GetRenderContext()->CreateTexture(settings));
        
        internalTexture->Bind();
        for (int f = 0; f < 6; ++f) {
            RHI::SubResource subResource(bytes[f], { static_cast<uint32_t>(f), 0, 0 });
            internalTexture->SetData(subResource);
        }
        internalTexture->Unbind();
        
        RHI::SamplerSettings samplerSettings = CreateFrom(options);
        internalSampler = DynamicAs<RHI::OpenGLSampler>(OpenGLDriver::GetCurrentDriver()->GetRenderContext()->CreateSampler(samplerSettings));
        
        // TODO: What to do with this?
        GLThrowIfFailed(glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS));
    }

    OpenGLCubemap::~OpenGLCubemap() {
        for(void* item: m_bytes) if (item) free(item);
    }

    void* OpenGLCubemap::GetHandler() {
        return (void *)(intptr_t)internalTexture->GetTextureID();
    }

    void OpenGLCubemap::Bind(uint32_t base) {
        internalTexture->Bind(base);
    }

    void OpenGLCubemap::Unbind(uint32_t base) {
        internalTexture->Unbind(base);
    }
}
