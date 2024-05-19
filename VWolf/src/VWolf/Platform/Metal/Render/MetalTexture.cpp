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
    Color Transform(TextureDefault textureDefault) {
        switch (textureDefault) {
        case TextureDefault::White: return Color(1, 1, 1, 1);
        case TextureDefault::Bump: return Color(0.5f, 0.5f, 1, 0.5f);
        case TextureDefault::Black: return Color(0, 0, 0, 1);
        case TextureDefault::Gray: return Color(0.5f, 0.5f, 0.5f, 1);
        case TextureDefault::Red: return Color(1, 0, 0, 1);
        }
    }

    MetalTexture2D::MetalTexture2D(TextureDefault textureDefault, uint32_t width, uint32_t height, TextureOptions options): Texture2D(textureDefault, width, height, options) {
        PopulateColor();

        Initialize(width, height, MTL::PixelFormat::PixelFormatRGBA32Float, 4 * 4, options);
        
    }
    MetalTexture2D::MetalTexture2D(const std::string filePath, TextureOptions options): Texture2D(filePath, options) {
        int channels, width, height;
        auto img = stbi_load(filePath.c_str(), &width, &height, &channels, 0);
        m_width = width;
        m_height = height;
        MTL::PixelFormat format = MTL::PixelFormat::PixelFormatRGBA32Float;
        float bytes = 4 * 4;
        if (channels == 4)
        {
            format = MTL::PixelFormat::PixelFormatRGBA8Unorm; // GL_RGBA8;
            bytes = 4;
        }
        m_data = img;

        Initialize(width, height, format, bytes, options);
    }

    void MetalTexture2D::Initialize(uint32_t width, uint32_t height, MTL::PixelFormat format, float bytes, TextureOptions options) {
        MTL::TextureDescriptor* descriptor = MTL::TextureDescriptor::texture2DDescriptor(format, width, height, false);
        texture = MetalDriver::GetCurrent()->GetDevice()->GetDevice()->newTexture(descriptor);
        MTL::Region region = MTL::Region(0, 0, width, height);
        texture->replaceRegion(region, 0, 0, m_data, bytes * width, bytes * width * height);
    }

    MetalTexture2D::~MetalTexture2D() {
        if (texture != nullptr)
            texture->release();
        if (m_data != nullptr) {
            free(m_data);
        }
    }

    void* MetalTexture2D::GetHandler() {
        return texture;
    }

    void MetalTexture2D::PopulateColor() {
        size_t size = sizeof(Vector4Float) * m_width * m_height;
        Vector4Float* data = (Vector4Float*)malloc(size);
        memset(data, 0, size);
        uint32_t index = 0;
        Vector4Float value = Transform(m_textureDefault);
        for (uint32_t column = 0; column < m_height; column++) {
            for (uint32_t row = 0; row < m_width; row++) {
                index = (column * m_height) + row;
                data[index] = value;
            }
        }
        m_data = data;
    }
#if defined(DEBUG) || defined(_DEBUG)
    void* MetalTexture2D::PopulateTest() {
        size_t size = sizeof(Vector4Float) * m_width * m_height;
        Vector4Float* data = (Vector4Float*)malloc(size);
        memset(data, 0, size);
        uint32_t index = 0;
        Vector4Float black(0, 0, 0, 1);
        Vector4Float white(1, 1, 1, 1);
        Vector4Float value = white;
        for (uint32_t column = 0; column < m_height; column++) {
            if (column % 32 == 0) {
                if (value.r == 1)
                    value = black;
                else if (value.r == 0)
                    value = white;
            }
            for (uint32_t row = 0; row < m_width; row++) {
                if (row % 32 == 0) {
                    if (value.r == 1)
                        value = black;
                    else if (value.r == 0)
                        value = white;
                }
                index = (column * m_height) + row;
                data[index] = value;
            }
        }
        return data;
    }
#endif

    MetalRenderTexture::MetalRenderTexture(uint32_t width, uint32_t height, bool isDepthOnly, TextureOptions options): RenderTexture(width, height, options), isDepthOnly(isDepthOnly) {
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
        return texture;
    }

    void MetalRenderTexture::Resize(uint32_t width, uint32_t height) {
        // TODO: What to do with this?
    }

    void MetalRenderTexture::Initialize() {
        MTL::PixelFormat rtvFormat = MetalDriver::GetCurrent()->GetSurface()->GetPixelFormat();
        MTL::TextureDescriptor* rtvDescriptor = MTL::TextureDescriptor::texture2DDescriptor(rtvFormat, m_width, m_height, false);
        rtvDescriptor->setUsage(MTL::TextureUsageRenderTarget | MTL::TextureUsageShaderRead);
        texture = MetalDriver::GetCurrent()->GetDevice()->GetDevice()->newTexture(rtvDescriptor);

        MTL::PixelFormat dsvFormat = MetalDriver::GetCurrent()->GetSurface()->GetDepthStencilPixelFormat();
        MTL::TextureDescriptor* dsvDescriptor = MTL::TextureDescriptor::texture2DDescriptor(dsvFormat, m_width, m_height, false);
        dsvDescriptor->setUsage(MTL::TextureUsageRenderTarget | MTL::TextureUsageShaderRead);
        depthTexture = MetalDriver::GetCurrent()->GetDevice()->GetDevice()->newTexture(dsvDescriptor);

        renderPassDescriptor = MTL::RenderPassDescriptor::alloc()->init();
        renderPassDescriptor->colorAttachments()->object(0)->setTexture(texture);
        renderPassDescriptor->depthAttachment()->setClearDepth(1.0f);
        renderPassDescriptor->depthAttachment()->setLoadAction(MTL::LoadAction::LoadActionClear);
        renderPassDescriptor->depthAttachment()->setStoreAction(MTL::StoreAction::StoreActionDontCare);
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

    MetalCubemap::MetalCubemap(TextureDefault textureDefault, uint32_t size, TextureOptions options): Cubemap(textureDefault, size, options) {
        Initialize(size, MTL::PixelFormat::PixelFormatRGBA32Float, options);
        PopulateColor();
        CopyData(4 * 4);
    }

    MetalCubemap::MetalCubemap(std::array<std::string, 6> paths, TextureOptions options): Cubemap(paths, options) {
        int channels, width, height;
        for (int index = 0; index < numberOfSides; index++) {
            auto img = stbi_load(paths[index].c_str(), &width, &height, &channels, 0);
            m_data[index] = img;
        }
        
        m_size = width;
        m_size = height;
        MTL::PixelFormat format = MTL::PixelFormat::PixelFormatRGBA32Float;
        float bytes = 4 * 4;
        if (channels == 4)
        {
            format = MTL::PixelFormat::PixelFormatRGBA8Unorm; // GL_RGBA8;
            bytes = 4;
        }
        Initialize(m_size, format, options);
        CopyData(bytes);
    }

    void MetalCubemap::CopyData(size_t numBytes) {
        MTL::Region region = MTL::Region(0, 0, m_size, m_size);
        for (int i = 0; i < m_data.size(); i++) {
            texture->replaceRegion(region, 0, i, m_data[i], numBytes * m_size, numBytes * m_size * m_size);
        }
    }

    MetalCubemap::~MetalCubemap() {
        if (texture != nullptr)
            texture->release();
        for (void* data: m_data) {
            if (data)
                free(data);
        }
    }

#if defined(DEBUG) || defined(_DEBUG)
    void MetalCubemap::PopulateTest() {
        Vector4Float value  = Transform(m_textureDefault);
        for (unsigned int i = 0; i < numberOfSides; i++)
        {
            size_t size = sizeof(Vector4Float) * m_size * m_size;
            Vector4Float* data = (Vector4Float*)malloc(size);
            memset(data, 0, size);
            uint32_t index = 0;
            for (uint32_t column = 0; column < m_size; column++) {
                for (uint32_t row = 0; row < m_size; row++) {
                    index = (column * m_size) + row;
                    data[index] = value;
                }
            }
            m_data[i] = data;
        }
    }

    void* MetalCubemap::PopulateTest(int checkIndex, Vector4Float otherColor) {
        size_t size = sizeof(Vector4Float) * m_size * m_size;
        Vector4Float* data = (Vector4Float*)malloc(size);
        memset(data, 0, size);
        uint32_t index = 0;
        Vector4Float white(1, 1, 1, 1);
        Vector4Float value = white;
        for (uint32_t column = 0; column < m_size; column++) {
            if (column % 32 == 0) {
                if (value[checkIndex] == 1)
                    value = otherColor;
                else if (value[checkIndex] == 0)
                    value = white;
            }
            for (uint32_t row = 0; row < m_size; row++) {
                if (row % 32 == 0) {
                    if (value[checkIndex] == 1)
                        value = otherColor;
                    else if (value[checkIndex] == 0)
                        value = white;
                }
                index = (column * m_size) + row;
                data[index] = value;
            }
        }
        return data;
    }
#endif

    void MetalCubemap::Initialize(uint32_t size, MTL::PixelFormat format, TextureOptions options) {
        MTL::TextureDescriptor* descriptor = MTL::TextureDescriptor::textureCubeDescriptor(format, size, false);
        texture = MetalDriver::GetCurrent()->GetDevice()->GetDevice()->newTexture(descriptor);
    }

    void* MetalCubemap::GetHandler() {
        return texture;
    }

    void MetalCubemap::PopulateColor() {
        std::array<Vector4Float, 6> colors = {
           Vector4Float(1, 0, 0, 1),
           Vector4Float(0, 1, 0, 1),
           Vector4Float(0, 0, 1, 1),
           Vector4Float(1, 1, 0, 1),
           Vector4Float(1, 0, 1, 1),
           Vector4Float(0, 1, 1, 1)
        };

        std::array<int, 6> indicesToCheck = {
            1,
            2,
            0,
            2,
            1,
            0
        };
        for (unsigned int i = 0; i < numberOfSides; i++)
        {
            m_data[i] = PopulateTest(indicesToCheck[i], colors[i]);
        }
    }
}

#endif
