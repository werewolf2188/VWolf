#pragma once

#include "VWolf/Core/Render/RenderStructs.h"
#include "OpenGLBuffer.h"

struct GLFWwindow;

namespace VWolf {
	class OpenGLVertexArray
	{
	public:
		OpenGLVertexArray(GLFWwindow* window);
		~OpenGLVertexArray();

		void Bind() const;
		void Unbind() const;

		void AddVertexBuffer(const Ref<OpenGLVertexBuffer>& vertexBuffer);
		void SetIndexBuffer(const Ref<OpenGLIndexBuffer>& indexBuffer);

		const std::vector<Ref<OpenGLVertexBuffer>>& GetVertexBuffers() const;
		const Ref<OpenGLIndexBuffer>& GetIndexBuffer() const;
	private:
		GLFWwindow* m_window = nullptr;
		unsigned int vertexArrayId = 0;
		unsigned int m_VertexBufferIndex = 0;
		Ref<OpenGLIndexBuffer> m_IndexBuffer = nullptr;
		std::vector<Ref<OpenGLVertexBuffer>> m_VertexBuffers;
	};
}