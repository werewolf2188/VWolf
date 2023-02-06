#pragma once

#include "Base.h"

#include "Window.h"
#include "Events/ApplicationEvent.h"

class Driver;

namespace VWolf {
	class CommandLineArguments {
	public:
		static std::vector<std::string> GetArguments();
		static void SetArguments(int args, const char** argv);
	private:
		static std::vector<std::string> m_arguments;
		static bool m_initialized;
	};

	class Application: public WindowEventCallback {
	public:
		static Application* GetApplication() { return m_application; };
	public:
		virtual ~Application();
		virtual void OnEvent(Event& evt) override;
		virtual void OnUpdate() = 0;
        virtual void OnDraw() = 0;
		virtual void OnGUI() = 0;
	public:
		void Run();
		Ref<Window> GetWindow();
		std::vector<std::string> GetArguments();
        DriverType GetDriverType() { return m_type; }
#ifdef VWOLF_CORE
		// TODO: Should I keep this?
		Driver* GetDriver() { return driver.get(); }
#endif
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
