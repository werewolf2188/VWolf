#pragma once

#include "Base.h"
#include "Events/Event.h"

#include "VWolf/Core/Events/MouseEvent.h"
#include "VWolf/Core/Events/KeyEvent.h"
#include "VWolf/Core/Events/ApplicationEvent.h"

#include <functional>

struct GLFWwindow;

#if defined(VWOLF_PLATFORM_MACOS) || defined(VWOLF_PLATFORM_IOS)
namespace NS {
    class Window;
    class View;
}
#elif defined(VWOLF_PLATFORM_WINDOWS)
struct HWND__;
#endif

namespace VWolf {
	class WindowEventCallback {
	public:
		virtual void OnEvent(Event& evt) = 0;
	};

	class Window {
	public:
		virtual ~Window() {};
		virtual void Initialize() = 0;
		virtual void OnUpdate() = 0;
		virtual void* GetNativeWindow() = 0;
        virtual WindowEventCallback& GetCallback() = 0;
		inline int GetWidth() { return width;  }
		inline int GetHeight() { return height; }
        inline void SetWidth(int width) { this->width = width; }
        inline void SetHeight(int height) { this->height = height; }
	protected:
		Window() {};
	protected: 
		int width = 0;
		int height = 0;
		bool vsync = false;
	};

    class GenericWindow: public Window, public MouseHandler, public KeyHandler {
    public:
        GenericWindow(DriverType driverType, InitConfiguration config, WindowEventCallback& callback, std::function<void()> initializer = [](){});
        virtual ~GenericWindow() override;
        virtual void Initialize() override;
        virtual void OnUpdate() override;
        virtual bool IsMouseButtonPressed(MouseCode button) override;
        virtual std::pair<float, float> GetMousePosition() override;
        virtual bool IsKeyPressed(KeyCode key) override;
        virtual void* GetNativeWindow() override;
    public:
        void InitializeEventHandler(GLFWwindow* m_window);
    public:
        virtual WindowEventCallback& GetCallback() override { return callback; }
        
#if defined(VWOLF_PLATFORM_MACOS) || defined(VWOLF_PLATFORM_IOS)
        inline NS::View* GetView() { return m_view; }
        inline void SetView(NS::View* view) { m_view = view; }
        inline NS::Window* GetCocoaWindow() { return reinterpret_cast<NS::Window*>(GetNativeWindow()); }
#elif defined(VWOLF_PLATFORM_WINDOWS)
		inline HWND__* GetWin32Window() { return reinterpret_cast<HWND__*>(GetNativeWindow()); }
#endif
    public:
        GLFWwindow* GetGLFWWindow() { return m_window; }
    public:
        
    private:
        std::function<void()> initializer;
        GLFWwindow *m_window;
        WindowEventCallback& callback;
#if defined(VWOLF_PLATFORM_MACOS) || defined(VWOLF_PLATFORM_IOS)
        NS::Window* m_nativeWindow;
        NS::View* m_view;
#elif defined(VWOLF_PLATFORM_WINDOWS)
		HWND__* m_nativeWindow;
#endif
    };
}
