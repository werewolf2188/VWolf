#include "vwpch.h"

#include <imgui.h>

#include "Driver.h"
#include "Application.h"
#include "Input.h"

#include "UI/UIManager.h"

#include "Log.h"

namespace VWolf {
	Application* Application::m_application = nullptr;

	Application::Application(DriverType type, InitConfiguration config) : driver(Driver::GetDriver(type))
	{
		VWOLF_CORE_ASSERT(config.width > 0);
		VWOLF_CORE_ASSERT(config.height > 0);

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
		UIManager::GetDefault().get()->Initialize();

		VWOLF_CORE_INFO("Running core application");
		m_running = true;
		bool show_demo_window = true;
		while (m_running) {
			// TODO: Replace by graphics context			
			driver->GetWindow()->Clear();

			UIManager::GetDefault().get()->NewFrame();
			ImGui::NewFrame();
			ImGui::ShowDemoWindow(&show_demo_window);
			UIManager::GetDefault().get()->Render();

			driver->OnUpdate();
			// TODO: Remove once its documented.
			/*if (Input::IsKeyPressed(KeyCode::A)) {
				VWOLF_CORE_DEBUG("A was pressed");
			}
			if (Input::IsMouseButtonPressed(MouseCode::Left)) {
				VWOLF_CORE_DEBUG("Left mouse button was pressed");
			}*/
			// VWOLF_CORE_DEBUG("Mouse position x: %0.2f, y: %0.2f", Input::GetMouseX(), Input::GetMouseY());
#if VWOLF_USE_EVENT_QUEUE
			EventQueue::defaultQueue->Dispatch();
#endif
		}
		UIManager::GetDefault().get()->Terminate();
	}

	Ref<Window>Application::GetWindow() {
		return driver->GetWindow();
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