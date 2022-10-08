#include "vwpch.h"

#include "OpenGLRenderAPI.h"

#include "VWolf/Core/Render/Shader.h"
#include "GLSLShader.h"

#include "VWolf/Core/Render/Buffer.h"
#include "OpenGLBuffer.h"

#include "VWolf/Core/Render/BufferGroup.h"
#include "OpenGLVertexArray.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace VWolf {
	OpenGLRenderAPI::OpenGLRenderAPI(GLFWwindow* window): m_window(window)
	{
		Shader::SetDefaultCreateMethod([this](const char* name,
                                              ShaderSource vertexShader,
                                              BufferLayout layout,
                                              std::initializer_list<ShaderSource> otherShaders,
                                              std::initializer_list<ShaderParameter> parameters,
                                              ShaderConfiguration configuration) {
			return CreateRef<GLSLShader>(m_window, name, vertexShader, layout, otherShaders, parameters, configuration);
		});

		VertexBuffer::SetDefaultCreateSizeMethod([this](uint32_t size) {
			return CreateRef<OpenGLVertexBuffer>(m_window, size);
		});

		VertexBuffer::SetDefaultCreateDataAndSizeMethod([this](float* vertices, uint32_t size) {
			return CreateRef<OpenGLVertexBuffer>(m_window, vertices, size);
		});

		IndexBuffer::SetDefaultCreateMethod([this](uint32_t* indices, uint32_t count) {
			return CreateRef<OpenGLIndexBuffer>(m_window, indices, count);
		});

		BufferGroup::SetDefaultCreateMethod([this]() {
			return CreateRef<OpenGLVertexArray>(m_window);
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