#pragma once

#include "VWolf/Core/Window.h"

struct GLFWwindow;

namespace VWolf {
	class GLFWWindow : public Window {
	public: // Inherits
		GLFWWindow(InitConfiguration config);
		virtual ~GLFWWindow() override;
		virtual void Initialize() override;
		virtual void Run() override;
	private:
		GLFWwindow* m_window;
		InitConfiguration config;
	};
}