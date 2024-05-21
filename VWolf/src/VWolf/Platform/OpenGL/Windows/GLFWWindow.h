#pragma once

#include "VWolf/Core/Window.h"
#include "VWolf/Core/Events/MouseEvent.h"
#include "VWolf/Core/Events/KeyEvent.h"

#include "VWolf/Platform/GLFWCommon.h"

struct GLFWwindow;

namespace VWolf {
	class GLFWWindow : public Window, public MouseHandler, public KeyHandler, public IWindowCallback {
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
        virtual WindowEventCallback& GetCallback() override { return callback; }
        virtual void SetWidth(int width) override { this->width = width; }
        virtual void SetHeight(int height) override { this->height = height; }
	private:
		GLFWwindow* m_window;
		WindowEventCallback& callback;
	};
}
