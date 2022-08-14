#pragma once

#include "VWolf/Core/Window.h"

struct GLFWwindow;

namespace VWolf {
	class GLFWWindow : public Window {
	public:
		GLFWWindow(InitConfiguration config);
		virtual ~GLFWWindow() override;
		virtual void Initialize() override;
	private:
		GLFWwindow* m_window;
		InitConfiguration config;
	};
}