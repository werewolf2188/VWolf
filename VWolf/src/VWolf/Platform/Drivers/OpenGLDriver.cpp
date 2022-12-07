#include "vwpch.h"
#include "OpenGLDriver.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "VWolf/Platform/Windows/GLFWWindow.h"
#include "VWolf/Core/UI/UIManager.h"

#include "VWolf/Platform/UI/OpenGLUIManager.h"

#include "VWolf/Core/Render/Renderer.h"
#include "VWolf/Platform/Render/OpenGLRenderer.h"

#include "VWolf/Core/Render/Shader.h"
#include "VWolf/Platform/Render/GLSLShader.h"

#include "VWolf/Core/Render/Buffer.h"
#include "VWolf/Platform/Render/OpenGLBuffer.h"

#include "VWolf/Core/Render/BufferGroup.h"
#include "VWolf/Platform/Render/OpenGLVertexArray.h"

#include "VWolf/Core/Time.h"

#define OPENGL_MAJOR_VERSION 4
#define OPENGL_MINOR_VERSION 1
#define OPENGL_PROFILE GLFW_OPENGL_CORE_PROFILE

namespace VWolf {
	// TODO: Error events
	static void GLFWErrorCallback(int error, const char* description)
	{
		VWOLF_CORE_ERROR("GLFW Error (%d): %s", error, description);
	}

	void OpenGLMessageCallback(
		unsigned source,
		unsigned type,
		unsigned id,
		unsigned severity,
		int length,
		const char* message,
		const void* userParam)
	{
		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:         VWOLF_CORE_FATAL(message); return;
		case GL_DEBUG_SEVERITY_MEDIUM:       VWOLF_CORE_ERROR(message); return;
		case GL_DEBUG_SEVERITY_LOW:          VWOLF_CORE_WARNING(message); return;
		case GL_DEBUG_SEVERITY_NOTIFICATION: VWOLF_CORE_TRACE(message); return;
		}

		VWOLF_CORE_ASSERT(false, "Unknown severity level!");
	}
	//

	class GLFWTime : public Time {
	protected:
		virtual float GetTime() override {
			return glfwGetTime();
		}
	};

	void OpenGLDriver::Initialize(InitConfiguration config, WindowEventCallback& callback)
	{
		this->callback = &callback;
		glfwInit();
		glfwSetErrorCallback(GLFWErrorCallback);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_MAJOR_VERSION);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_MINOR_VERSION);
		glfwWindowHint(GLFW_OPENGL_PROFILE, OPENGL_PROFILE);
		//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

		window = CreateRef<GLFWWindow>(config, callback);
		UIManager::SetDefault(CreateRef<OpenGLUIManager>((GLFWwindow*)window->GetNativeWindow()));
        Renderer::SetRenderer(CreateScope<OpenGLRenderer>((GLFWwindow *)window->GetNativeWindow()));
		Time::SetTimeImplementation(CreateRef<GLFWTime>());

		Shader::SetDefaultCreateMethod([this](const char* name,
			ShaderSource vertexShader,
			BufferLayout layout,
			std::initializer_list<ShaderSource> otherShaders,
			std::vector<ShaderParameter> parameters,
			ShaderConfiguration configuration) {
				return CreateRef<GLSLShader>((GLFWwindow*)window->GetNativeWindow(), name, vertexShader, layout, otherShaders, parameters, configuration);
		});

		VertexBuffer::SetDefaultCreateSizeMethod([this](uint32_t size) {
			return CreateRef<OpenGLVertexBuffer>((GLFWwindow*)window->GetNativeWindow(), size);
		});

		VertexBuffer::SetDefaultCreateDataAndSizeMethod([this](float* vertices, uint32_t size) {
			return CreateRef<OpenGLVertexBuffer>((GLFWwindow*)window->GetNativeWindow(), vertices, size);
		});

		IndexBuffer::SetDefaultCreateMethod([this](uint32_t* indices, uint32_t count) {
			return CreateRef<OpenGLIndexBuffer>((GLFWwindow*)window->GetNativeWindow(), indices, count);
		});

		BufferGroup::SetDefaultCreateMethod([this]() {
			return CreateRef<OpenGLVertexArray>((GLFWwindow*)window->GetNativeWindow());
		});

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
			glfwTerminate();
		}
		window->Initialize();
#ifdef DEBUG
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        if (OPENGL_MINOR_VERSION > 5) {
            glDebugMessageCallback(OpenGLMessageCallback, nullptr);

            glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
        }
#endif
	}

	void OpenGLDriver::Shutdown()
	{
		window.reset();
		glfwTerminate();
	}

	void OpenGLDriver::OnUpdate() {
		window->OnUpdate();
		glfwSwapBuffers((GLFWwindow*)window->GetNativeWindow());
	}

	void OpenGLDriver::OnEvent(Event& evt) {
		callback->OnEvent(evt);
	}

    void OpenGLDriver::Resize(unsigned int m_Width, unsigned int m_Height) {
#ifdef VWOLF_PLATFORM_MACOS
        int width;
        int height;
        glfwGetFramebufferSize((GLFWwindow*)window->GetNativeWindow(), &width, &height);
        glViewport(0, 0, width, height);
#else
        glViewport(0, 0, m_Width, m_Height);
#endif
    }
}
