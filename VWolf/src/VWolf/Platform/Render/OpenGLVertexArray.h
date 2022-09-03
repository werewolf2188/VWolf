#pragma once

#include "VWolf/Core/Render/BufferGroup.h"

struct GLFWwindow;

namespace VWolf {
	class OpenGLVertexArray: public BufferGroup
	{
	public:
		OpenGLVertexArray(GLFWwindow* window);
		virtual ~OpenGLVertexArray();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) override;
		virtual void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) override;

		virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const override;
		virtual const Ref<IndexBuffer>& GetIndexBuffer() const override;
	private:
		GLFWwindow* m_window = nullptr;
		unsigned int vertexArrayId = 0;
		unsigned int m_VertexBufferIndex = 0;
		Ref<IndexBuffer> m_IndexBuffer = nullptr;
		std::vector<Ref<VertexBuffer>> m_VertexBuffers;
	};
}