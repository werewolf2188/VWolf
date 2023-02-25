#include "vwpch.h"
#include "OpenGLBuffer.h"
#include "VWolf/Platform/OpenGL/Core/GLCore.h"

namespace VWolf {
	OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size): m_size(size)
	{
		VWOLF_CORE_ASSERT(size);
        GLThrowIfFailed(glGenBuffers(1, &vertexBufferId));
        GLThrowIfFailed(glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId));
        GLThrowIfFailed(glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW));
	}
	OpenGLVertexBuffer::OpenGLVertexBuffer(void* vertices, uint32_t size): m_vertices(vertices), m_size(size)
	{
		VWOLF_CORE_ASSERT(size);
//        VWOLF_DUMP_TYPED_ARRAY(vertices, size, float);
        GLThrowIfFailed(glGenBuffers(1, &vertexBufferId));
        GLThrowIfFailed(glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId));
        GLThrowIfFailed(glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW));
	}
	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
        GLThrowIfFailed(glDeleteBuffers(1, &vertexBufferId));
	}
	void OpenGLVertexBuffer::Bind() const
	{
        GLThrowIfFailed(glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId));
	}
	void OpenGLVertexBuffer::Unbind() const
	{
        GLThrowIfFailed(glBindBuffer(GL_ARRAY_BUFFER, 0));
	}
	void OpenGLVertexBuffer::SetData(const void* data, uint32_t size)
	{
        GLThrowIfFailed(glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId));
        GLThrowIfFailed(glBufferSubData(GL_ARRAY_BUFFER, 0, size, data));
	}
	const BufferLayout& OpenGLVertexBuffer::GetLayout() const
	{
		return layout;
	}
	void OpenGLVertexBuffer::SetLayout(const BufferLayout& layout)
	{
		this->layout = layout;
	}
	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t* indices, uint32_t count): m_indices(indices), m_count(count)
	{
		VWOLF_CORE_ASSERT(count);
        GLThrowIfFailed(glGenBuffers(1, &indexBufferId));
//        VWOLF_DUMP_ARRAY(indices, count);

		// GL_ELEMENT_ARRAY_BUFFER is not valid without an actively bound VAO
		// Binding with GL_ARRAY_BUFFER allows the data to be loaded regardless of VAO state. 
        GLThrowIfFailed(glBindBuffer(GL_ARRAY_BUFFER, indexBufferId));
        GLThrowIfFailed(glBufferData(GL_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW));
	}
	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
        GLThrowIfFailed(glDeleteBuffers(1, &indexBufferId));
	}
	void OpenGLIndexBuffer::Bind() const
	{
        GLThrowIfFailed(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId));
	}
	void OpenGLIndexBuffer::Unbind() const
	{
        GLThrowIfFailed(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	}
	uint32_t OpenGLIndexBuffer::GetCount() const
	{
		return m_count;
	}
}
