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
        MetalTexture2D(TextureDefault textureDefault, uint32_t width, uint32_t height, TextureOptions options = {});
        MetalTexture2D(const std::string filePath, TextureOptions options = {});
        virtual ~MetalTexture2D();
        virtual void* GetHandler() override;
    protected:
        virtual void PopulateColor() override;
    #if defined(DEBUG) || defined(_DEBUG)
    private:
        void* PopulateTest();
    #endif
        void Initialize(uint32_t width, uint32_t height, MTL::PixelFormat format, float bytes, TextureOptions options = {});
    private:
        MTL::Texture* texture = nullptr;
        bool hasBeenUpload = false;
        void* m_data = nullptr;
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
        MetalCubemap(TextureDefault textureDefault, uint32_t size, TextureOptions options = {});
        MetalCubemap(std::filesystem::path path, TextureOptions options = {});
        virtual ~MetalCubemap();
        virtual void* GetHandler() override;
    protected:
        virtual void PopulateColor() override;
    #if defined(DEBUG) || defined(_DEBUG)
    private:
        //void PopulateTest(GLuint id, int checkIndex, Vector4Float otherColor);
        void PopulateTest();
        void* PopulateTest(int checkIndex, Color otherColor);
    #endif
        void Initialize(uint32_t size, MTL::PixelFormat format, TextureOptions options = {});
        void CopyData(size_t numBytes);
    private:
        MTL::Texture* texture = nullptr;
        bool hasBeenUpload = false;
        std::array<void*, 6> m_data;
        uint32_t numberOfSides = 6;
    };
    
}
