#include "vwpch.h"
#include "OpenGLUniformBuffer.h"

#include "GLSLShader.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace VWolf {
	OpenGLUniformBuffer::OpenGLUniformBuffer(GLFWwindow* window, Ref<Shader> shader, const std::string name, uint32_t size, uint32_t binding) : UniformBuffer(shader, name, size, binding), m_window(window)
	{
		GLuint programId = dynamic_cast<GLSLShader*>(shader.get())->GetProgramID();
		unsigned int uniformBlock = glGetUniformBlockIndex(programId, name.c_str());
		glUniformBlockBinding(programId, uniformBlock, binding);

		glGenBuffers(1, &uniformBufferId);
        glBindBuffer(GL_UNIFORM_BUFFER, uniformBufferId);
		glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_STATIC_DRAW); // TODO: investigate usage hint
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        glBindBufferBase(GL_UNIFORM_BUFFER, binding, uniformBufferId);
	}
	OpenGLUniformBuffer::~OpenGLUniformBuffer()
	{
		glDeleteBuffers(1, &uniformBufferId);
	}
	void OpenGLUniformBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
	{
        glBindBuffer(GL_UNIFORM_BUFFER, uniformBufferId);
		glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
}