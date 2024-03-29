#pragma once

#include "VWolf/Core/Render/RenderStructs.h"

struct GLFWwindow;

namespace VWolf {
	class OpenGLVertexBuffer
	{
	public:
		OpenGLVertexBuffer(uint32_t size);
		OpenGLVertexBuffer(void* vertices, uint32_t size);
		~OpenGLVertexBuffer();

		void Bind() const;
		void Unbind() const;

		void SetData(const void* data, uint32_t size);

		const BufferLayout& GetLayout() const;
		void SetLayout(const BufferLayout& layout);
	private:
		BufferLayout layout;
		unsigned int vertexBufferId = 0;
        void* m_vertices;
		uint32_t m_size;
	};

	class OpenGLIndexBuffer
	{
	public:
		OpenGLIndexBuffer(uint32_t* indices, uint32_t count);
		~OpenGLIndexBuffer();

		void Bind() const;
		void Unbind() const;

		uint32_t GetCount() const;
	private:
		unsigned int indexBufferId = 0;
		uint32_t* m_indices;
		uint32_t m_count;
	};
}
