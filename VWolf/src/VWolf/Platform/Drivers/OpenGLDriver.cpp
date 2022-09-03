#include "vwpch.h"
#include "OpenGLDriver.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "VWolf/Platform/Windows/GLFWWindow.h"
#include "VWolf/Core/UI/UIManager.h"

#include "VWolf/Platform/UI/OpenGLUIManager.h"

#include "VWolf/Platform/Render/OpenGLRenderAPI.h"
#include "VWolf/Core/Render/Renderer.h"

#include "VWolf/Core/Math/Math.h"
#include "VWolf/Platform/Math/GLMMath.h"

#define OPENGL_MAJOR_VERSION 4
#define OPENGL_MINOR_VERSION 6
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
		UIManager::SetDefault(CreateRef<OpenGLUIManager>(((GLFWWindow*)window.get())->GetContainerWindow()));
		Renderer::SetRenderAPI(CreateScope<OpenGLRenderAPI>(((GLFWWindow*)window.get())->GetContainerWindow()));
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
			glfwTerminate();
		}
		window->Initialize();
#ifdef DEBUG
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(OpenGLMessageCallback, nullptr);

		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
#endif
		// These should be settings
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_CULL_FACE);		
		glCullFace(GL_FRONT); 

		glEnable(GL_DEPTH_TEST);
		//
		Math::SetInstance(CreateRef<GLMMath>());
	}

	void OpenGLDriver::Shutdown()
	{
		glfwTerminate();
	}

	void OpenGLDriver::OnUpdate() {
		window->OnUpdate();
		glfwSwapBuffers(((GLFWWindow*)window.get())->GetContainerWindow());
	}

	void OpenGLDriver::OnEvent(Event& evt) {
		callback->OnEvent(evt);
	}
}