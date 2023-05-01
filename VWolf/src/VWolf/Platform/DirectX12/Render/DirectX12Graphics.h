#pragma once

#include "VWolf/Core/Render/Graphics.h"

#include "HLSLShader.h"
#include "DirectX12Buffer.h"
#include "DirectX12BufferGroup.h"

namespace VWolf {
    class DirectX12Graphics : public Graphics {
    public:
        DirectX12Graphics() = default;
        virtual ~DirectX12Graphics() override {};
    public:
        void ClearResources(bool forceRelease);
        void Build();
    protected:
        virtual void DrawMeshImpl(MeshData & mesh, Vector4Float position, Vector4Float rotation, Material & material, Ref<Camera> camera = nullptr) override;
        virtual void RenderMeshImpl(MeshData & mesh, MatrixFloat4x4 transform, Material & material, Ref<Camera> camera = nullptr) override;
        virtual void ClearColorImpl(Color color) override;
        virtual void ClearImpl() override;
        // TODO: Not sure about this one
        virtual void AddLightImpl(Light & light) override;
        virtual void DrawGridImpl() override;
        virtual void BeginFrameImpl() override;
        virtual void EndFrameImpl() override;
        virtual void SetRenderTextureImpl(Ref<RenderTexture> renderTexture) override;
    private:
        void BindToRenderTexture();
        void UnbindToRenderTexture();       
    private:
        std::deque<std::pair<UINT64, Ref<DirectX12BufferGroup>>> groups;
        // TODO: Plan later
        std::vector<Light> lights;       
        int frame = 0;
        int shapes = 0;
        Ref<HLSLShader> gridShader;
    };
}