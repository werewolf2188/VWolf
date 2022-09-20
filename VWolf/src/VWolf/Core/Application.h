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
		virtual ~Application();
		void Run();
		Ref<Window> GetWindow();
		virtual void OnEvent(Event& evt) override;
		virtual void OnUpdate() = 0;
		virtual void OnUIUpate() = 0;
	protected: 
		Application() = delete;
		Application(DriverType type, InitConfiguration config);	
		bool OnWindowResize(WindowResizeEvent& e);
	private:
		bool OnWindowClose(WindowCloseEvent& e);
	private:
		static Application* m_application;
		DriverType m_type;
		Scope<Driver> driver;
		bool m_running = false;
		bool m_minimized = false;
	};
}