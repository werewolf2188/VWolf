#pragma once

#include "VWolf/Core/Base.h"
// TODO: Merge camera classes into one
#include "Camera.h"
#include "Shader.h"
#include "Material.h"
#include "RenderStructs.h"

#include "VWolf/Core/Math/VMath.h"
#include "VWolf/Core/Time.h"

namespace VWolf {
    struct RenderItem;

    class Graphics {
    public:
        virtual ~Graphics() = default;
    public:
        static void SetRenderTexture(Ref<RenderTexture> renderTexture);
        static void DrawMesh(MeshData& mesh, Vector4Float position, Vector4Float rotation, Material& material, Ref<Camera> camera = nullptr);
        static void RenderMesh(MeshData& mesh, MatrixFloat4x4 transform, Material& material, Ref<Camera> camera = nullptr);
        static void ClearColor(Color color);
        static void Clear();
        // TODO: Not sure about this one
        static void AddLight(Light& light);
#ifdef VWOLF_CORE
        static void SetGraphicsImpl(Ref<Graphics> graphics) { graphicsImpl = graphics; }
        static Ref<Graphics> SetGraphicsImpl() { return graphicsImpl; }
        static void BeginFrame();
        static void EndFrame();
#endif
    protected:
        virtual void DrawMeshImpl(MeshData& mesh, Vector4Float position, Vector4Float rotation, Material& material, Ref<Camera> camera = nullptr) = 0;
        virtual void RenderMeshImpl(MeshData& mesh, MatrixFloat4x4 transform, Material& material, Ref<Camera> camera = nullptr) = 0;
        virtual void ClearColorImpl(Color color) = 0;
        virtual void ClearImpl() = 0;
        // TODO: Not sure about this one
        virtual void AddLightImpl(Light& light) = 0;
        virtual void BeginFrameImpl() = 0;
        virtual void EndFrameImpl() = 0;
        virtual void SetRenderTextureImpl(Ref<RenderTexture> renderTexture) = 0;
    protected:
        // TODO: Think about how to deal with render textures
        Ref<RenderTexture> renderTexture;
    private:
        static Ref<Graphics> graphicsImpl;
    };
}
