#include "vwpch.h"
#include "OpenGLDriver.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "VWolf/Platform/Windows/GLFWWindow.h"

#define GL_MAJOR_VERSION 4
#define GL_MINOR_VERSION 6
#define GL_PROFILE GLFW_OPENGL_CORE_PROFILE

namespace VWolf {
	void OpenGLDriver::Initialize()
	{
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GL_MAJOR_VERSION);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GL_MINOR_VERSION);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GL_PROFILE);
		//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

		window = new GLFWWindow();
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
}