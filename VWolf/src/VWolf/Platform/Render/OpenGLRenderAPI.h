#pragma once

#include "VWolf/Core/Render/RenderAPI.h"

struct GLFWwindow;

namespace VWolf {
	class OpenGLRenderAPI : public RenderAPI {
	public:
		OpenGLRenderAPI(GLFWwindow* window);

		virtual void Begin(Camera& camera, Ref<Shader> shader) override;
		virtual void ClearColor(Color color) override;
		virtual void Clear() override;
		virtual void End() override;
		virtual void Resize(unsigned int m_Width, unsigned int m_Height) override;
		virtual void DrawIndexed(const Ref<BufferGroup>& group, uint32_t indexCount) override;
	public:
		GLFWwindow* m_window;
	};
}