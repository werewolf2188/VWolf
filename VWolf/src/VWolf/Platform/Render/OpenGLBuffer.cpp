#include "vwpch.h"
#include "OpenGLBuffer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace VWolf {
	OpenGLVertexBuffer::OpenGLVertexBuffer(GLFWwindow* window, uint32_t size): m_window(window), m_size(size)
	{
		VWOLF_CORE_ASSERT(size);
		glGenBuffers(1, &vertexBufferId);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
	}
	OpenGLVertexBuffer::OpenGLVertexBuffer(GLFWwindow* window, float* vertices, uint32_t size) : m_window(window), m_vertices(vertices), m_size(size)
	{
		VWOLF_CORE_ASSERT(size);
//        VWOLF_DUMP_TYPED_ARRAY(vertices, size, float);
		glGenBuffers(1, &vertexBufferId);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	}
	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		glDeleteBuffers(1, &vertexBufferId);
	}
	void OpenGLVertexBuffer::Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	}
	void OpenGLVertexBuffer::Unbind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	void OpenGLVertexBuffer::SetData(const void* data, uint32_t size)
	{
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	}
	const BufferLayout& OpenGLVertexBuffer::GetLayout() const
	{
		return layout;
	}
	void OpenGLVertexBuffer::SetLayout(const BufferLayout& layout)
	{
		this->layout = layout;
	}
	OpenGLIndexBuffer::OpenGLIndexBuffer(GLFWwindow* window, uint32_t* indices, uint32_t count): m_window(window), m_indices(indices), m_count(count)
	{
		VWOLF_CORE_ASSERT(count);
		glGenBuffers(1, &indexBufferId);
//        VWOLF_DUMP_ARRAY(indices, count);

		// GL_ELEMENT_ARRAY_BUFFER is not valid without an actively bound VAO
		// Binding with GL_ARRAY_BUFFER allows the data to be loaded regardless of VAO state. 
		glBindBuffer(GL_ARRAY_BUFFER, indexBufferId);
		glBufferData(GL_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
	}
	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		glDeleteBuffers(1, &indexBufferId);
	}
	void OpenGLIndexBuffer::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
	}
	void OpenGLIndexBuffer::Unbind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	uint32_t OpenGLIndexBuffer::GetCount() const
	{
		return m_count;
	}
}