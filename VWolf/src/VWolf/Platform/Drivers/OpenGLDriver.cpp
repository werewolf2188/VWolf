#include "vwpch.h"
#include "OpenGLDriver.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "VWolf/Platform/Windows/GLFWWindow.h"

#define OPENGL_MAJOR_VERSION 4
#define OPENGL_MINOR_VERSION 6
#define OPENGL_PROFILE GLFW_OPENGL_CORE_PROFILE

namespace VWolf {
	void OpenGLDriver::Initialize(InitConfiguration config, WindowEventCallback& callback)
	{
		this->callback = &callback;
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_MAJOR_VERSION);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_MINOR_VERSION);
		glfwWindowHint(GLFW_OPENGL_PROFILE, OPENGL_PROFILE);
		//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

		window = new GLFWWindow(config, callback);
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
			glfwTerminate();
		}
		window->Initialize();
	}

	void OpenGLDriver::Shutdown()
	{
		delete window;
		glfwTerminate();
	}

	void OpenGLDriver::OnUpdate() {
		window->OnUpdate();
		glfwSwapBuffers(((GLFWWindow*)window)->GetContainerWindow());
	}

	void OpenGLDriver::OnEvent(Event& evt) {
		callback->OnEvent(evt);
	}
}