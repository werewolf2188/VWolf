#include "vwpch.h"
#include "GLFWWindow.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace VWolf {
	GLFWWindow::GLFWWindow(InitConfiguration config): Window()
	{
		this->config = config;
		m_window = glfwCreateWindow(config.width, config.height, config.title, NULL, NULL);
		if (m_window == NULL)
		{
			std::cout << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
		}
		glfwMakeContextCurrent(m_window);
		glfwSetWindowUserPointer(m_window, this);
	}

	GLFWWindow::~GLFWWindow()
	{
		glfwDestroyWindow(m_window);
	}

	void GLFWWindow::Initialize()
	{
		// TODO: Move
		glViewport(0, 0, config.width, config.height);

		// TODO: Initialize UI		
	}

	void GLFWWindow::Run() {
		while (!glfwWindowShouldClose(m_window))
		{
			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			glViewport(0, 0, config.width, config.height);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glfwSwapBuffers(m_window);
			glfwPollEvents();
		}
	}
}