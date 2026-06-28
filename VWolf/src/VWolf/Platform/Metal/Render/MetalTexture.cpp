//
//  MetalTexture.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 5/11/24.
//

#include "vwpch.h"

#if defined(VWOLF_PLATFORM_MACOS) || defined(VWOLF_PLATFORM_IOS)
#include "MetalTexture.h"

#include "VWolf/Platform/Metal/MetalDriver.h"

#include "stb_image/stb_image.h"

namespace VWolf {
    MetalTexture2D::MetalTexture2D(void * bytes, uint32_t width, uint32_t height, TextureOptions options): PTexture2D(bytes, width, height, options) {
        m_width = width;
        m_height = height;
        MTL::PixelFormat format = MTL::PixelFormat::PixelFormatRGBA32Float;
        float bytesNum = 4 * 4;

        Initialize(width, height, format, bytesNum, options);
    }

    void MetalTexture2D::Initialize(uint32_t width, uint32_t height, MTL::PixelFormat format, float bytes, TextureOptions options) {
        MTL::TextureDescriptor* descriptor = MTL::TextureDescriptor::texture2DDescriptor(format, width, height, false);
        texture = MetalDriver::GetCurrent()->GetDevice()->GetDevice()->newTexture(descriptor);
        MTL::Region region = MTL::Region(0, 0, width, height);
        texture->replaceRegion(region, 0, 0, m_bytes, bytes * width, bytes * width * height);
    }

    MetalTexture2D::~MetalTexture2D() {
        if (texture != nullptr)
            texture->release();
        if (m_bytes != nullptr) {
            free(m_bytes);
        }
    }

    void* MetalTexture2D::GetHandler() {
        return texture;
    }

    MetalRenderTexture::MetalRenderTexture(uint32_t width, uint32_t height, bool isDepthOnly, TextureOptions options): PRenderTexture(width, height, options), isDepthOnly(isDepthOnly) {
        Initialize();
    }
    
    MetalRenderTexture::~MetalRenderTexture() {
        if (texture != nullptr)
            texture->release();
        if (depthTexture != nullptr)
            depthTexture->release();
        if (renderPassDescriptor != nullptr)
            renderPassDescriptor->release();
    }

    void* MetalRenderTexture::GetHandler() {
        return isDepthOnly ? depthTexture: texture;
    }

    void MetalRenderTexture::Resize(uint32_t width, uint32_t height) {
        // TODO: What to do with this?
        if (m_width == width && m_height == height) return;
        if (texture != nullptr)
            texture->release();
        if (depthTexture != nullptr)
            depthTexture->release();
        if (renderPassDescriptor != nullptr)
            renderPassDescriptor->release();
        m_width = width;
        m_height = height;
        Initialize();
    }

    void MetalRenderTexture::Initialize() {
        if (!isDepthOnly) {
            MTL::PixelFormat rtvFormat = MetalDriver::GetCurrent()->GetSurface()->GetPixelFormat();
            MTL::TextureDescriptor* rtvDescriptor = MTL::TextureDescriptor::texture2DDescriptor(rtvFormat, m_width, m_height, false);
            rtvDescriptor->setUsage(MTL::TextureUsageRenderTarget | MTL::TextureUsageShaderRead);
            texture = MetalDriver::GetCurrent()->GetDevice()->GetDevice()->newTexture(rtvDescriptor);
        }

        MTL::PixelFormat dsvFormat = MetalDriver::GetCurrent()->GetSurface()->GetDepthStencilPixelFormat();
        MTL::TextureDescriptor* dsvDescriptor = MTL::TextureDescriptor::texture2DDescriptor(dsvFormat, m_width, m_height, false);
        dsvDescriptor->setUsage(MTL::TextureUsageRenderTarget | MTL::TextureUsageShaderRead);
        depthTexture = MetalDriver::GetCurrent()->GetDevice()->GetDevice()->newTexture(dsvDescriptor);

        renderPassDescriptor = MTL::RenderPassDescriptor::alloc()->init();
        if (!isDepthOnly)
            renderPassDescriptor->colorAttachments()->object(0)->setTexture(texture);
        renderPassDescriptor->depthAttachment()->setClearDepth(1.0f);
        renderPassDescriptor->depthAttachment()->setLoadAction(MTL::LoadAction::LoadActionClear);
        renderPassDescriptor->depthAttachment()->setStoreAction(isDepthOnly ? MTL::StoreAction::StoreActionStore : MTL::StoreAction::StoreActionDontCare);
        renderPassDescriptor->depthAttachment()->setTexture(depthTexture);
        renderPassDescriptor->stencilAttachment()->setTexture(depthTexture);
    }

    void MetalRenderTexture::Prepare() {
        rtvCommandBuffer = MetalDriver::GetCurrent()->GetCommand()->GetCommandQueue()->commandBuffer();
    }

    MTL::RenderCommandEncoder* MetalRenderTexture::StartEncoder() {
        if (startedEncoding) return rtvEncoder;
        rtvEncoder = rtvCommandBuffer->renderCommandEncoder(renderPassDescriptor);
        startedEncoding = true;
        return rtvEncoder;
    }

    void MetalRenderTexture::Commit() {
        rtvEncoder->endEncoding();
        rtvCommandBuffer->commit();
        startedEncoding = false;
    }

    MetalCubemap::MetalCubemap(std::array<void *, 6> bytes, uint32_t size, TextureOptions options): PCubemap(bytes, size, options) {
        MTL::PixelFormat format = MTL::PixelFormat::PixelFormatRGBA32Float;
        float bytesNum = 4 * 4;
        Initialize(m_size, format, options);
        CopyData(bytesNum);
    }

    void MetalCubemap::CopyData(size_t numBytes) {
        MTL::Region region = MTL::Region(0, 0, m_size, m_size);
        for (int i = 0; i < m_bytes.size(); i++) {
            texture->replaceRegion(region, 0, i, m_bytes[i], numBytes * m_size, numBytes * m_size * m_size);
        }
    }

    MetalCubemap::~MetalCubemap() {
        if (texture != nullptr)
            texture->release();
        for (void* data: m_bytes) {
            if (data)
                free(data);
        }
    }

    void MetalCubemap::Initialize(uint32_t size, MTL::PixelFormat format, TextureOptions options) {
        MTL::TextureDescriptor* descriptor = MTL::TextureDescriptor::textureCubeDescriptor(format, size, false);
        texture = MetalDriver::GetCurrent()->GetDevice()->GetDevice()->newTexture(descriptor);
    }

    void* MetalCubemap::GetHandler() {
        return texture;
    }
}

#endif
