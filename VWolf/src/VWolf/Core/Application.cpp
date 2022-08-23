#include "vwpch.h"

#include "Driver.h"
#include "Application.h"

#include "Log.h"

namespace VWolf {
	Application* Application::m_application = nullptr;

	Application::Application(DriverType type, InitConfiguration config) : driver(Driver::GetDriver(type))
	{
		m_application = this;
		VWOLF_CORE_INFO("Initializing core application");
		VWOLF_CORE_DEBUG("Starting with driver: %s", DriverName(type));
		driver->Initialize(config, *this);

		// Adding subscribers
#if VWOLF_USE_EVENT_QUEUE
		EventQueue::defaultQueue->Subscribe<WindowCloseEvent>(VWOLF_BIND_EVENT_FN(Application::OnWindowClose));
		EventQueue::defaultQueue->Subscribe<WindowResizeEvent>(VWOLF_BIND_EVENT_FN(Application::OnWindowResize));
#endif
	}

	Application::~Application()
	{
		VWOLF_CORE_INFO("Shutting down core application");
		driver->Shutdown();
	}

	void Application::Run() {
		VWOLF_CORE_INFO("Running core application");
		m_running = true;
		while (m_running) {
			// TODO: Replace by graphics context			
			driver->GetWindow()->Clear();
			driver->OnUpdate();
#if VWOLF_USE_EVENT_QUEUE
			EventQueue::defaultQueue->Dispatch();
#endif
		}
	}

	void Application::OnEvent(Event& evt) {		
#if !VWOLF_USE_EVENT_QUEUE
		Dispatch<WindowCloseEvent>(evt, VWOLF_BIND_EVENT_FN(Application::OnWindowClose));
		Dispatch<WindowResizeEvent>(evt, VWOLF_BIND_EVENT_FN(Application::OnWindowResize));
#endif
	}

	bool Application::OnWindowClose(WindowCloseEvent& e) {
		m_running = !m_running;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e) {
		VWOLF_CORE_DEBUG(e.ToString());
		m_minimized = e.GetHeight() == 0 && e.GetWidth() == 0;
		return false;
	}
}