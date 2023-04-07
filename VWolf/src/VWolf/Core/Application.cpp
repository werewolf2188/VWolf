#include "vwpch.h"

#include <imgui.h>

#include "Driver.h"
#include "Application.h"
#include "Input.h"

#include "UI/UIManager.h"
#include "Render/Graphics.h"

#include "Time.h"

#include "Log.h"

namespace VWolf {
	std::vector<std::string> CommandLineArguments::GetArguments()
	{
		return m_arguments;
	}
	void CommandLineArguments::SetArguments(int args, const char** argv)
	{
		if (m_initialized) return;
		for (int i = 0; i < args; i++)
			m_arguments.push_back(argv[i]);
		m_initialized = true;
	}

	std::vector<std::string> CommandLineArguments::m_arguments;

	bool CommandLineArguments::m_initialized = false;

	Application* Application::m_application = nullptr;

	Application::Application(DriverType type, InitConfiguration config) : m_type(type), driver(Driver::GetDriver(type))
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
        UIManager::GetDefault()->Initialize();
	}

	Application::~Application()
	{
		VWOLF_CORE_INFO("Shutting down core application");
		driver->Shutdown();
		// Log::ClearLogObjects(); // TODO: Take control of memory
	}

	void Application::Run() {
		VWOLF_CORE_INFO("Running core application");
		m_running = true;
		
		while (m_running) {	
			Time::Tick();
			if (!m_minimized) {
				OnUpdate();
				Graphics::BeginFrame();
				OnDraw();				
				UIManager::GetDefault()->NewFrame();
                ImGui::NewFrame();
                OnGUI();
				UIManager::GetDefault()->Render();
				Graphics::EndFrame();
			}			

			driver->OnUpdate();
#if VWOLF_USE_EVENT_QUEUE
			EventQueue::defaultQueue->Dispatch();
#endif
		}
		UIManager::GetDefault()->Terminate();
	}

	Ref<Window>Application::GetWindow() {
		return driver->GetWindow();
	}

	std::vector<std::string> Application::GetArguments()
	{
		return CommandLineArguments::GetArguments();
	}

	void Application::OnEvent(Event& evt) {		
#if !VWOLF_USE_EVENT_QUEUE
		Dispatch<WindowCloseEvent>(evt, VWOLF_BIND_EVENT_FN(Application::OnWindowClose));
		Dispatch<WindowResizeEvent>(evt, VWOLF_BIND_EVENT_FN(Application::OnWindowResize));
#endif
		if (UIManager::GetDefault())
			UIManager::GetDefault()->OnEvent(evt);
	}

	bool Application::OnWindowClose(WindowCloseEvent& e) {
		m_running = !m_running;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e) {
		VWOLF_CORE_DEBUG(e.ToString());
		m_minimized = e.GetHeight() == 0 && e.GetWidth() == 0;
        driver->Resize(e.GetWidth(), e.GetHeight());
		return false;
	}
}