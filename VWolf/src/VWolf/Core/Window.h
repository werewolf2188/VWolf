#pragma once

#include "Base.h"
#include "Events/Event.h"

#include "VWolf/Core/Events/MouseEvent.h"
#include "VWolf/Core/Events/KeyEvent.h"
#include "VWolf/Core/Events/ApplicationEvent.h"

struct GLFWwindow;

#if defined(VWOLF_PLATFORM_MACOS) || defined(VWOLF_PLATFORM_IOS)
namespace NS {
    class Window;
    class View;
}
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

		inline int GetWidth() { return width;  }
		inline int GetHeight() { return height; }
	protected:
		Window() {};
	protected: 
		int width = 0;
		int height = 0;
		bool vsync = false;
	};

    class IWindowCallback {
    public:
        void InitializeEventHandler(GLFWwindow* m_window);
    public:
        virtual WindowEventCallback& GetCallback() = 0;
        virtual void SetWidth(int width) = 0;
        virtual void SetHeight(int height) = 0;
    };

    
    KeyCode GetKeyCodeFrom(int key);
    int GetKeyFrom(KeyCode key);
    KeyMods GetKeyModsFrom(int mods);
    int GetMouseFrom(MouseCode button);
    MouseCode GetMouseCode(int button);

    class GenericWindow: public Window, public MouseHandler, public KeyHandler, public IWindowCallback {
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
        virtual WindowEventCallback& GetCallback() override { return callback; }
        virtual void SetWidth(int width) override { this->width = width; }
        virtual void SetHeight(int height) override { this->height = height; }
#if defined(VWOLF_PLATFORM_MACOS) || defined(VWOLF_PLATFORM_IOS)
        inline NS::View* GetView() { return m_view; }
        inline void SetView(NS::View* view) { m_view = view; }
        inline NS::Window* GetCocoaWindow() { return reinterpret_cast<NS::Window*>(GetNativeWindow()); }
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
#endif
    };
}
