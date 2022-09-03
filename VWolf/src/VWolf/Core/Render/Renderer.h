#pragma once

#include "VWolf/Core/Base.h"
#include "RenderAPI.h"
#include "Camera.h"
#include "Shader.h"
#include "Buffer.h"
#include "BufferGroup.h"

#include "VWolf/Core/Math/Matrices.h"

namespace VWolf {
	struct RendererData;

	class Renderer {
	public:
		static void Begin(Camera& camera, Ref<Shader> shader);
		static void ClearColor(Color color);
		static void Clear();
		static void End();
		static void Resize(unsigned int m_Width, unsigned int m_Height);
		static void SetRenderAPI(Scope<RenderAPI> renderApi) { m_renderApi = std::move(renderApi); }
		static bool Ready() { return (bool)m_renderApi; }
		static void Submit(Ref<BufferGroup> group, MatrixFloat4x4 transform);
	private:
		static Scope<RenderAPI> m_renderApi;
		static Scope<RendererData> data;
	};
}