#include "vwpch.h"

#include "OpenGLRenderAPI.h"

#include "VWolf/Core/Render/Shader.h"
#include "GLSLShader.h"

#include "VWolf/Core/Render/Buffer.h"
#include "OpenGLBuffer.h"

#include "VWolf/Core/Render/BufferGroup.h"
#include "OpenGLVertexArray.h"

#include "VWolf/Core/Render/UniformBuffer.h"
#include "OpenGLUniformBuffer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace VWolf {
	OpenGLRenderAPI::OpenGLRenderAPI(GLFWwindow* window): m_window(window)
	{
		Shader::SetDefaultCreateMethod([window](const std::string& name, BufferLayout layout) {
			return CreateRef<GLSLShader>(name, layout, window);
		});

		VertexBuffer::SetDefaultCreateSizeMethod([window](uint32_t size) {
			return CreateRef<OpenGLVertexBuffer>(window, size);
		});

		VertexBuffer::SetDefaultCreateDataAndSizeMethod([window](float* vertices, uint32_t size) {
			return CreateRef<OpenGLVertexBuffer>(window, vertices, size);
		});

		IndexBuffer::SetDefaultCreateMethod([window](uint32_t* indices, uint32_t count) {
			return CreateRef<OpenGLIndexBuffer>(window, indices, count);
		});

		BufferGroup::SetDefaultCreateMethod([window]() {
			return CreateRef<OpenGLVertexArray>(window);
		});

		UniformBuffer::SetDefaultCreateMethod([window](Ref<Shader> shader, const std::string name, uint32_t size, uint32_t binding) {
			return CreateRef<OpenGLUniformBuffer>(window, shader, name, size, binding);
		});
	}
	void OpenGLRenderAPI::Begin(Camera& camera, Ref<Shader> shader)
	{
		//TODO: I still need to double bind in OpenGL, so maybe not
		//shader->Bind();
	}
	void OpenGLRenderAPI::ClearColor(Color color)
	{
		glClearColor(color.r, color.b, color.g, color.a);
	}
	void OpenGLRenderAPI::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	void OpenGLRenderAPI::End()
	{
	}
	void OpenGLRenderAPI::Resize(unsigned int m_Width, unsigned int m_Height)
	{		
		glViewport(0, 0, m_Width, m_Height);		
	}
	void OpenGLRenderAPI::DrawIndexed(const Ref<BufferGroup>& group, uint32_t indexCount)
	{
		group->Bind();
		uint32_t count = indexCount ? indexCount : group->GetIndexBuffer()->GetCount();
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
	}
}