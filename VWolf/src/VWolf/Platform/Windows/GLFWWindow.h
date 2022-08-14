#pragma once

#include "VWolf/Core/Window.h"

struct GLFWwindow;

namespace VWolf {
	class GLFWWindow : public Window {
	public:
		GLFWWindow();
		virtual ~GLFWWindow() override;
		virtual void Initialize() override;
	private:
		GLFWwindow* m_window;
		// TODO: Remove
		int width = 800;
		int height = 600;
	};
}