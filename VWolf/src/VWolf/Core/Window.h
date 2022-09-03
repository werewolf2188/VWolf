#pragma once

#include "Base.h"
#include "Events/Event.h"
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

		inline int GetWidth() { return width;  }
		inline int GetHeight() { return height; }
	protected:
		Window() {};
	protected: 
		int width = 0;
		int height = 0;
		bool vsync = false;
	};
}