#pragma once

#include "VWolf/Core/Base.h"
// TODO: Merge camera classes into one
#include "PerspectiveCamera.h"
#include "Shader.h"
#include "Buffer.h"
#include "BufferGroup.h"

#include "VWolf/Core/Math/Math.h"

namespace VWolf {
    struct RenderItem;

    class Renderer {
    public:
        virtual ~Renderer() = default;
    public:
        static void Begin(Ref<PerspectiveCamera> camera);
        static void ClearColor(Color color);
        static void Clear();
        static void SetMaterial(AbstractMaterial& material);
        static void SetLight(Light& light);
        static void DrawMesh(MeshData& meshData, MatrixFloat4x4 transform);
        static void End();
#ifdef VWOLF_CORE
        static void SetRenderer(Scope<Renderer> renderer) { rendererImpl = std::move(renderer); }
#endif
    protected:
        virtual void ProcessItems() = 0;
        CameraPass GetCameraPass();
    protected:
        Ref<PerspectiveCamera> m_camera;
        Color backgroundColor;
        AbstractMaterial* material;
        Light* light;
        std::vector<Ref<RenderItem>> items;
        bool clearColor = false;
        bool clearDepthStencil = false;
    private:
        static Scope<Renderer> rendererImpl;
    };
}
