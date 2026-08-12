//
//  MetalGraphics.hpp
//  VWolf
//
//  Created by Enrique Ricalde on 4/19/24.
//

#pragma once

#include "VWolf/Core/Render/InternalGraphics.h"

#include "VWolf/Platform/Metal/Core/Core.h"

#include "VWolf/Platform/Metal/Render/MetalBufferGroup.h"
#include "VWolf/Platform/Metal/Render/MetalTexture.h"

namespace VWolf {

    class MetalGraphics: public InternalGraphics {
    public:
        MetalGraphics() = default;
        virtual ~MetalGraphics() override {};
    public:
        virtual void Initialize() override;
    public:
        MTL::CommandBuffer* GetCommandBuffer() { return commandBuffer; }
        MTL::RenderCommandEncoder* GetRenderCommandEncoder() { return encoder; }
    protected:
        void ClearColorImpl(Color color);
        void ClearImpl();

        virtual void EndProcessingFrame() override;
        virtual void BeginProcessingFrame() override;
    protected:
        virtual void DrawShadowMap() override;
        virtual void DrawQueue() override;
        virtual void DrawPostProcess() override;
    private:
        std::map<UUID, Ref<MetalBufferGroup>> bufferGroups;
        std::map<UUID, Matrix4x4> objectTransforms;
        
        NS::AutoreleasePool* pool;
        MTL::CommandBuffer* commandBuffer;
        MTL::RenderCommandEncoder* encoder;
        Ref<MetalTexture2D> emptyShadowMap;
        Ref<MetalRenderTexture> shadowMap;
        std::map<std::string, uint32_t> constantBufferIndexPerShader;
        int itemsCount = 0;
    };
}
