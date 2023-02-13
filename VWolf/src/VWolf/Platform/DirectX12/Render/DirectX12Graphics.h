#pragma once

#include "VWolf/Core/Render/Graphics.h"

#include "HLSLShader.h"
#include "DirectX12Buffer.h"
#include "DirectX12BufferGroup.h"

struct HWND__;
struct HINSTANCE__;

struct DirectX12Context;

namespace VWolf {
    class DirectX12Graphics : public Graphics {
    public:
        DirectX12Graphics() = default;
        virtual ~DirectX12Graphics() override {};
    /*public:
        void Build();*/
    protected:
        virtual void DrawMeshImpl(MeshData & mesh, Vector4Float position, Vector4Float rotation, Material & material, Ref<Camera> camera = nullptr) override;
        virtual void RenderMeshImpl(MeshData & mesh, MatrixFloat4x4 transform, Material & material, Ref<Camera> camera = nullptr) override;
        virtual void ClearColorImpl(Color color) override;
        virtual void ClearImpl() override;
        // TODO: Not sure about this one
        virtual void AddLightImpl(Light & light) override;
        virtual void DrawGridImpl() override;
    /*private:
        void BindToRenderTexture();
        void UnbindToRenderTexture();*/
    private:
        // TODO: Plan later
        std::vector<Light> lights;
        std::vector<Ref<DirectX12BufferGroup>> groups;
        int frame = 0;
    };
}