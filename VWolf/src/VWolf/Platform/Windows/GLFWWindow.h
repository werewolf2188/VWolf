#pragma once

#include "VWolf/Core/Window.h"
#include "VWolf/Core/Events/MouseEvent.h"
#include "VWolf/Core/Events/KeyEvent.h"

struct GLFWwindow;

namespace VWolf {
	class GLFWWindow : public Window, public MouseHandler, public KeyHandler {
	public: // Inherits
		GLFWWindow(InitConfiguration config, WindowEventCallback& callback);
		virtual ~GLFWWindow() override;
		virtual void Initialize() override;
		virtual void OnUpdate() override;
		virtual bool IsMouseButtonPressed(MouseCode button) override;
		virtual std::pair<float, float> GetMousePosition() override;
		virtual bool IsKeyPressed(KeyCode key) override;
		virtual void* GetNativeWindow() override { return m_window; };
	public:
		inline WindowEventCallback& GetCallback() { return callback; }
		inline void SetWidth(int width) { this->width = width; }
		inline void SetHeight(int height) { this->height = height; }		
	private:
		GLFWwindow* m_window;
		WindowEventCallback& callback;
	};
}