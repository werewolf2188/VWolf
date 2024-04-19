#include "vwpch.h"
#include "GLFWWindow.h"

#include "VWolf/Core/Events/ApplicationEvent.h"

#include "VWolf/Platform/OpenGL/Core/GLCore.h"

namespace VWolf {
	GLFWWindow::GLFWWindow(InitConfiguration config, WindowEventCallback& callback) : Window(), callback(callback)
	{
		this->width = config.width;
		this->height = config.height;

		m_window = glfwCreateWindow(width, height, config.title, NULL, NULL);
		if (m_window == NULL)
		{
			std::cout << "Failed to create GLFW window" << std::endl;
			WindowCloseEvent evt;
			GetCallback().OnEvent(evt);
			glfwTerminate();
		}
		const GLFWvidmode& mode = *glfwGetVideoMode(glfwGetPrimaryMonitor());
		int w = mode.width, h = mode.height;
		glfwSetWindowPos(m_window, (w / 2) - (width / 2), (h / 2) - (height / 2));
		if (config.maximize)
			glfwMaximizeWindow(m_window);
		glfwMakeContextCurrent(m_window);

        InitializeEventHandler(m_window);
	}

	GLFWWindow::~GLFWWindow()
	{
		glfwDestroyWindow(m_window);
	}

	void GLFWWindow::Initialize()
	{
#ifdef VWOLF_PLATFORM_MACOS
        int width;
        int height;
        glfwGetFramebufferSize(m_window, &width, &height);
        GLThrowIfFailed(glViewport(0, 0, width, height));
#else
        GLThrowIfFailed(glViewport(0, 0, width, height));
#endif
	}

	void GLFWWindow::OnUpdate() {
		glfwPollEvents();
	}

	bool GLFWWindow::IsMouseButtonPressed(MouseCode button) {
		return glfwGetMouseButton(m_window, GetMouseFrom(button)) == GLFW_PRESS;
	}

	std::pair<float, float> GLFWWindow::GetMousePosition() {
		double xpos, ypos;
		glfwGetCursorPos(m_window, &xpos, &ypos);
		// Not sure if clamping is the right way to go
		xpos = std::clamp((int)xpos, 0, width);
		ypos = std::clamp((int)ypos, 0, height);
		return std::make_pair<float, float>((float)xpos, (float)ypos);
	}

	bool GLFWWindow::IsKeyPressed(KeyCode key) {
		return glfwGetKey(m_window, GetKeyFrom(key)) == GLFW_PRESS;
	}
}
