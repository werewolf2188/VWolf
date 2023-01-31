#pragma once

#include "VWolf/Core/Render/RenderStructs.h"
#include "OpenGLBuffer.h"

struct GLFWwindow;

namespace VWolf {
	class OpenGLVertexArray
	{
	public:
        OpenGLVertexArray();
		OpenGLVertexArray(const Ref<OpenGLVertexBuffer>& vertexBuffer);
        OpenGLVertexArray(const Ref<OpenGLVertexBuffer>& vertexBuffer, const BufferLayout& layout);
		~OpenGLVertexArray();

		void Bind() const;
		void Unbind() const;
    private:
        void Build(const Ref<OpenGLVertexBuffer>& vertexBuffer);
	private:
        const BufferLayout& m_layout;
		unsigned int vertexArrayId = 0;
		unsigned int m_VertexBufferIndex = 0;
	};
}
