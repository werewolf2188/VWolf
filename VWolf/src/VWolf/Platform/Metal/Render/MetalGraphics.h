//
//  MetalGraphics.hpp
//  VWolf
//
//  Created by Enrique Ricalde on 4/19/24.
//

#pragma once

#include "VWolf/Core/Render/Graphics.h"

#include "VWolf/Platform/Metal/Core/Core.h"

#include "VWolf/Platform/Metal/Render/MetalBufferGroup.h"

namespace VWolf {
    class MetalGraphics: public Graphics {
    public:
        MetalGraphics() = default;
        virtual ~MetalGraphics() override {};
    public:
        void Initialize();
    public:
        MTL::CommandBuffer* GetCommandBuffer() { return commandBuffer; }
        MTL::RenderCommandEncoder* GetRenderCommandEncoder() { return encoder; }
    protected:
        virtual void DrawMeshImpl(MeshData& mesh, Vector4Float position, Vector4Float rotation, Material& material, Ref<Camera> camera = nullptr) override;
        virtual void RenderMeshImpl(MeshData& mesh, MatrixFloat4x4 transform, Material& material, Ref<Camera> camera = nullptr) override;
        virtual void ClearColorImpl(Color color) override;
        virtual void ClearImpl() override;
        // TODO: Not sure about this one
        virtual void AddLightImpl(Light& light) override;
        virtual void BeginFrameImpl() override;
        virtual void EndFrameImpl() override;
        virtual void SetRenderTextureImpl(Ref<RenderTexture> renderTexture) override;
        virtual void BeginSceneImpl() override;
        virtual void EndSceneImpl() override;
    protected:
        virtual void DrawShadowMap() override;
        virtual void DrawQueue() override;
        virtual void DrawPostProcess() override;
    private:
        std::vector<Ref<MetalBufferGroup>> bufferGroups;
        std::vector<MatrixFloat4x4> objectTransforms;
        // MARK: Per frame
        std::vector<Ref<RenderItem>> items;
        NS::AutoreleasePool* pool;
        MTL::CommandBuffer* commandBuffer;
        MTL::RenderCommandEncoder* encoder;
    };
}
