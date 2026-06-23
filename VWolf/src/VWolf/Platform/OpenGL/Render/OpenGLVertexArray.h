#pragma once

#include "VWolf/Core/Render/Mesh.h"
#include "OpenGLBuffer.h"

struct GLFWwindow;

namespace VWolf {
	class OpenGLVertexArray
	{
	public:
        OpenGLVertexArray(std::vector<AttributeDescriptor> layout);
        OpenGLVertexArray(const Ref<OpenGLVertexBuffer>& vertexBuffer, std::vector<AttributeDescriptor> layout);
		~OpenGLVertexArray();

		void Bind() const;
		void Unbind() const;
    private:
        void Build(const Ref<OpenGLVertexBuffer>& vertexBuffer);
	private:
        std::vector<AttributeDescriptor> m_layout;
		unsigned int vertexArrayId = 0;
		unsigned int m_VertexBufferIndex = 0;
	};
}
