#pragma once

#include "VWolf/Core/Render/UniformBuffer.h"

struct GLFWwindow;

namespace VWolf {
	class OpenGLUniformBuffer : public UniformBuffer {
	public:
		OpenGLUniformBuffer(GLFWwindow* window, Ref<Shader> shader, const std::string name, uint32_t size, uint32_t binding);
		virtual ~OpenGLUniformBuffer() override;
		virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) override;
	private:
		GLFWwindow* m_window = nullptr;
		unsigned int uniformBufferId = 0;
	};
}