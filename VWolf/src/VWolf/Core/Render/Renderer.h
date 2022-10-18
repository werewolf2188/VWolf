#pragma once

#include "VWolf/Core/Base.h"
#include "RenderAPI.h"
#include "Camera.h"
#include "Shader.h"
#include "Buffer.h"
#include "BufferGroup.h"

#include "VWolf/Core/Math/Math.h"

namespace VWolf {

	class TestRenderer {
	public:
		static void Begin(Camera& camera, Ref<Shader> shader);
		static void ClearColor(Color color);
		static void Clear();
		static void End();
		static void SetRenderAPI(Scope<RenderAPI> renderApi) { m_renderApi = std::move(renderApi); }
		static bool Ready() { return (bool)m_renderApi; }
		static void Submit(Ref<BufferGroup> group, MatrixFloat4x4 transform);
	private:
		static Scope<RenderAPI> m_renderApi;
	};

    struct RenderItem;

    class Renderer {
    public:
        virtual ~Renderer() = default;
    public:
        static void Begin(Ref<Camera> camera);
        static void ClearColor(Color color);
        static void Clear();
        static void SetShader(const char* shaderName);
        static void DrawMesh(MeshData& meshData, MatrixFloat4x4 transform);
        static void End();
#ifdef VWOLF_CORE
        static void SetRenderer(Scope<Renderer> renderer) { rendererImpl = std::move(renderer); }
#endif
    protected:
        virtual void ProcessItems() = 0;
    protected:
        Ref<Camera> m_camera;
        Color backgroundColor;
        std::string shaderName;
        std::vector<Ref<RenderItem>> items;
        bool clearColor = false;
        bool clearDepthStencil = false;
    private:
        static Scope<Renderer> rendererImpl;
    };
}
