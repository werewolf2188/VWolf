#pragma once

#include "VWolf/Core/Window.h"

struct GLFWwindow;

namespace VWolf {
	class GLFWWindow : public Window {
	public: // Inherits
		GLFWWindow(InitConfiguration config);
		virtual ~GLFWWindow() override;
		virtual void Initialize() override;
		//TODO: Remove
		virtual bool ShouldClose() override;
		virtual void Clear() override;
	private:
		GLFWwindow* m_window;
		InitConfiguration config;
	};
}