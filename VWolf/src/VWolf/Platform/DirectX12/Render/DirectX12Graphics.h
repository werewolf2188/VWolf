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
        virtual void DrawMeshImpl(Ref<Mesh> mesh1, Vector4 position, Vector4 rotation, Material & material, Ref<Camera> camera = nullptr) override;
        virtual void RenderMeshImpl(Ref<Mesh> mesh1, Matrix4x4 transform, Material & material, Ref<Camera> camera = nullptr) override;
        virtual void ClearColorImpl(Color color) override;
        virtual void ClearImpl() override;
        // TODO: Not sure about this one
        virtual void AddLightImpl(Light & light) override;
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
        std::deque<std::pair<UINT64, Ref<DirectX12BufferGroup>>> groups;
		std::deque<std::pair<UINT64, Ref<Texture>>> textureGroups;
        std::vector<Ref<RenderItem>> items;
        Ref<DirectX12RenderTexture> shadowMap;
        Ref<DirectX12Texture2D> emptyShadowMap;
        // TODO: Plan later
        std::vector<Light> lights;    
        std::vector<Matrix4x4> spaces;
        int frame = 0;
        int shapes = 0;
    };
}
