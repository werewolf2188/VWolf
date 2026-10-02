#pragma once

#include "Base.h"
#include "Events/Event.h"

#include "VWolf/Core/Events/MouseEvent.h"
#include "VWolf/Core/Events/KeyEvent.h"
#include "VWolf/Core/Events/ApplicationEvent.h"

#include <functional>

namespace VWolf {

    class Monitor {
    public:
        virtual uint32_t GetVideoModeWidth() = 0;
        virtual uint32_t GetVideoModeHeight() = 0;
    };

	class Window {
	public:
		virtual ~Window() {};
		virtual void OnUpdate() = 0;
		virtual void* GetNativeWindow() = 0;
        virtual Ref<Monitor> GetMonitor() = 0;
		inline int GetWidth() { return width;  }
		inline int GetHeight() { return height; }
        inline void SetWidth(int width) { this->width = width; }
        inline void SetHeight(int height) { this->height = height; }
        inline int GetFramebufferWidth() { return framebufferWidth;  }
        inline int GetFramebufferHeight() { return framebufferHeight; }
        inline void SetFramebufferWidth(int width) { this->framebufferWidth = width; }
        inline void SetFramebufferHeight(int height) { this->framebufferHeight = height; }
	protected:
		Window() {};
	protected: 
		int width = 0;
		int height = 0;
        int framebufferWidth = 0;
        int framebufferHeight = 0;
		bool vsync = false;
	};

    Ref<Window> CreateGenericWindow(DriverType driverType, InitConfiguration config);
}
