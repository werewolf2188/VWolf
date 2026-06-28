//
//  MetalTexture.hpp
//  VWolf
//
//  Created by Enrique Ricalde on 5/11/24.
//

#pragma once

#include "VWolf/Platform/PTexture.h"
#include "VWolf/Core/Math/VMath.h"
#include "VWolf/Platform/Metal/Core/Core.h"

namespace VWolf {
    class MetalTexture2D : public PTexture2D {
    public:
        MetalTexture2D(void * bytes, uint32_t width, uint32_t height, TextureOptions options = {});
        virtual ~MetalTexture2D();
        virtual void* GetHandler() override;
        void Initialize(uint32_t width, uint32_t height, MTL::PixelFormat format, float bytes, TextureOptions options = {});
    private:
        MTL::Texture* texture = nullptr;
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
        MTL::RenderCommandEncoder* StartEncoder();
        void Commit();
    public:
        MTL::Texture* GetTexture() const { return texture; }
        MTL::RenderPassDescriptor* GetRenderPassDescriptor() const { return renderPassDescriptor; }
    private:
        void Initialize();
    private:
        MTL::CommandBuffer* rtvCommandBuffer;
        MTL::RenderCommandEncoder* rtvEncoder;
        MTL::Texture* texture = nullptr;
        MTL::Texture* depthTexture = nullptr;
        MTL::RenderPassDescriptor* renderPassDescriptor;
        bool isDepthOnly = false;
        bool startedEncoding = false;
    };

    class MetalCubemap : public PCubemap {
    public:        
        MetalCubemap(std::array<void *, 6> bytes, uint32_t size, TextureOptions options = {});
        virtual ~MetalCubemap();
        virtual void* GetHandler() override;
        void Initialize(uint32_t size, MTL::PixelFormat format, TextureOptions options = {});
        void CopyData(size_t numBytes);
    private:
        MTL::Texture* texture = nullptr;
        bool hasBeenUpload = false;
        uint32_t numberOfSides = 6;
    };
}
