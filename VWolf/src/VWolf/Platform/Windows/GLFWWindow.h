#pragma once

#include "VWolf/Core/Window.h"

struct GLFWwindow;

namespace VWolf {
	class GLFWWindow : public Window {
	public: // Inherits
		GLFWWindow(InitConfiguration config, WindowEventCallback& callback);
		virtual ~GLFWWindow() override;
		virtual void Initialize() override;
		virtual void OnUpdate() override;
		//TODO: Remove
		virtual void Clear() override;
	public:
		inline WindowEventCallback& GetCallback() { return callback; }
		inline GLFWwindow* GetContainerWindow() { return m_window; }
		inline void SetWidth(int width) { this->width = width; }
		inline void SetHeight(int height) { this->height = height; }
	private:
		GLFWwindow* m_window;
		WindowEventCallback& callback;
	};
}