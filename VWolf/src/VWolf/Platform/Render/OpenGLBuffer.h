#pragma once

#include "VWolf/Core/Render/Buffer.h"

struct GLFWwindow;

namespace VWolf {
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(GLFWwindow* window, uint32_t size);
		OpenGLVertexBuffer(GLFWwindow* window, float* vertices, uint32_t size);
		virtual ~OpenGLVertexBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void SetData(const void* data, uint32_t size) override;

		virtual const BufferLayout& GetLayout() const override;
		virtual void SetLayout(const BufferLayout& layout) override;
	private:
		GLFWwindow* m_window = nullptr;
		BufferLayout layout;
		unsigned int vertexBufferId = 0;
	};

	class OpenGLIndexBuffer: public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(GLFWwindow* window, uint32_t* indices, uint32_t count);
		virtual ~OpenGLIndexBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual uint32_t GetCount() const override;
	private:
		GLFWwindow* m_window = nullptr;
		unsigned int indexBufferId = 0;
	};
}