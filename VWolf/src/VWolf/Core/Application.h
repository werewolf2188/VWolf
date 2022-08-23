#pragma once

#include "Base.h"

#include "Window.h"
#include "Events/ApplicationEvent.h"

class Driver;

namespace VWolf {
	class Application: public WindowEventCallback {
	public:
		static Application* GetApplication() { return m_application; };
	public:
		~Application();
		void Run();
		virtual void OnEvent(Event& evt) override;
	protected: 
		Application() = delete;
		Application(DriverType type, InitConfiguration config);	
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);
	private:
		static Application* m_application;
		Driver* driver;
		bool m_running = false;
		bool m_minimized = false;
	};
}