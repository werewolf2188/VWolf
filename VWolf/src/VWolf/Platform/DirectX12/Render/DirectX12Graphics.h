#pragma once

#include "VWolf/Core/Render/InternalGraphics.h"

#include "HLSLShader.h"
#include "DirectX12Buffer.h"
#include "DirectX12BufferGroup.h"
#include "DirectX12Texture.h"

namespace VWolf {
    class DirectX12Graphics : public InternalGraphics {
    public:
        DirectX12Graphics() = default;
        virtual ~DirectX12Graphics() override {};
    public:
        void ClearResources(bool forceRelease);
        void Initialize();
    protected:
        void ClearColorImpl(Color color);
        void ClearImpl();

        virtual void EndProcessingFrame() override;
        virtual void SetRenderTextureImpl(Ref<RenderTexture> renderTexture) override;
        virtual void BeginProcessingFrame() override;
    protected:
        virtual void DrawShadowMap() override;
        virtual void DrawQueue() override;
        virtual void DrawPostProcess() override;
    private:
        std::deque<std::pair<UINT64, Ref<DirectX12BufferGroup>>> groups;
		std::deque<std::pair<UINT64, Ref<Texture>>> textureGroups;
        Ref<DirectX12RenderTexture> shadowMap;
        Ref<DirectX12Texture2D> emptyShadowMap;
    };
}
