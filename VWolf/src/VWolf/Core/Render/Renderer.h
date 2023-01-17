#pragma once

#include "VWolf/Core/Base.h"
// TODO: Merge camera classes into one
#include "Camera.h"
#include "Shader.h"
#include "Material.h"
#include "RenderStructs.h"

#include "VWolf/Core/Math/Math.h"
#include "VWolf/Core/Time.h"

namespace VWolf {
    struct RenderItem;

    class Renderer {
    public:
        virtual ~Renderer() = default;
    public:
        static void Begin(Ref<Camera> camera);
        static void ClearColor(Color color);
        static void Clear();
        static void SetMaterial(AbstractMaterial& material);
//        static void SetLight(Light& light);
        static void AddLight(Light& light);
        static void DrawMesh(MeshData& meshData, MatrixFloat4x4 transform);
        static void End();
#ifdef VWOLF_CORE
        static void SetRenderer(Scope<Renderer> renderer) { rendererImpl = std::move(renderer); }
#endif
    protected:
        virtual void ProcessItems() = 0;
        CameraPass GetCameraPass();
    protected:
        Ref<Camera> m_camera;
        Color backgroundColor;
        AbstractMaterial* material;
//        Light* light;
        std::vector<Light> lights;
        std::vector<Ref<RenderItem>> items;
        bool clearColor = false;
        bool clearDepthStencil = false;
    private:
        static Scope<Renderer> rendererImpl;
    };

    class Graphics {
    public:
        virtual ~Graphics() = default;
    public:
        static void DrawMesh(MeshData& mesh, Vector4Float position, Vector4Float rotation, Material& material, Ref<Camera> camera = nullptr);
        static void RenderMesh(MeshData& mesh, MatrixFloat4x4 transform, Material& material, Ref<Camera> camera = nullptr);
        static void ClearColor(Color color);
        static void Clear();
        // TODO: Not sure about this one
        static void AddLight(Light& light);
#ifdef VWOLF_CORE
        static void SetGraphicsImpl(Scope<Graphics> graphics) { graphicsImpl = std::move(graphics); }
#endif
    protected:
        virtual void DrawMeshImpl(MeshData& mesh, Vector4Float position, Vector4Float rotation, Material& material, Ref<Camera> camera = nullptr) = 0;
        virtual void RenderMeshImpl(MeshData& mesh, MatrixFloat4x4 transform, Material& material, Ref<Camera> camera = nullptr) = 0;
        virtual void ClearColorImpl(Color color) = 0;
        virtual void ClearImpl() = 0;
        // TODO: Not sure about this one
        virtual void AddLightImpl(Light& light) = 0;
    private:
        static Scope<Graphics> graphicsImpl;
    };
}
