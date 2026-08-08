#include "vwpch.h"

#include <imgui.h>

#include "Driver.h"
#include "Application.h"
#include "Input.h"

#include "UI/UIManager.h"
#include "Render/GraphicsContext.h"
#include "Render/InternalGraphics.h"

#include "Time.h"

#include "Log.h"

namespace VWolf {

    class Lifecycle {
    public:

        void BeginProcessingFrame() {
			if (InternalGraphics::Singleton() != nullptr)
				InternalGraphics::Singleton()->BeginProcessingFrame();
        }
        
        void EndProcessingFrame() {
			if (InternalGraphics::Singleton() != nullptr)
				InternalGraphics::Singleton()->EndProcessingFrame();

			GraphicsContext::Reset();
        }
    };

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

    Scope<Application> Application::m_application = nullptr;

    bool Application::_isPlaying = false;

	Application::Application(DriverType type, InitConfiguration config): m_type(type), config(config), driver(Driver::GetDriver(type))
	{
        lifecycle = CreateScope<Lifecycle>();
        std::filesystem::path currentPath = CommandLineArguments::GetArguments()[0];
        currentPath = currentPath.remove_filename();
        if (std::filesystem::current_path() != currentPath)
            std::filesystem::current_path(currentPath);

		VWOLF_CORE_ASSERT(config.width > 0);
		VWOLF_CORE_ASSERT(config.height > 0);

		VWOLF_CORE_DEBUG("Starting with driver: %s", DriverName(type));
		
		// Adding subscribers
		EventQueue::DefaultQueue->Subscribe<WindowCloseEvent>(VWOLF_BIND_EVENT_FN(Application::OnWindowClose));
		EventQueue::DefaultQueue->Subscribe<WindowResizeEvent>(VWOLF_BIND_EVENT_FN(Application::OnWindowResize));
	}

	Application::~Application() { }

    void Application::Initialize() {
        VWOLF_CORE_INFO("Initializing core application");
        driver->Initialize(config);
        UIManager::GetDefault()->Initialize();
    }

    void Application::Shutdown() {
        VWOLF_CORE_INFO("Shutting down core application");
        driver->Shutdown();
        // Log::ClearLogObjects(); // TODO: Take control of memory
    }

	void Application::Run() {
		VWOLF_CORE_INFO("Running core application");
		m_running = true;

        updateThread = std::thread(&Application::Update, this);
        Render();
        updateThread.join();
	}

    void Application::Quit() {
        NotifyShutdown();
    }

    void Application::NotifyShutdown() {
        m_running = false;
        m_frameCV.notify_all();
    }

    void Application::Update() {
        uint64_t updateFrame = 0;

        while (m_running) {
            {
                std::unique_lock<std::mutex> lock(m_frameMutex);
                m_frameCV.wait(lock, [this, updateFrame] {
                    return m_eventsFrame > updateFrame || !m_running.load();
                });

                if (!m_running) {
                    break;
                }

                updateFrame = m_eventsFrame;
            }

            // Consume events produced by the main thread, then build draw commands.
            EventQueue::DefaultQueue->Dispatch([&](){ return !m_running.load(); });

            if (!m_running) {
                break;
            }

            if (!m_minimized) {
                OnUpdate();
                OnDraw();
            }

            {
                std::lock_guard<std::mutex> lock(m_frameMutex);
                m_commandsFrame = updateFrame;
            }
            m_frameCV.notify_one();
        }

        // Unblock the render thread if it is waiting for a final frame.
        m_frameCV.notify_one();
    }

    void Application::Render() {
        while (m_running) {
            // Produce: poll window events into EventQueue.
            Time::Tick();
            driver->OnUpdate();

            uint64_t renderFrame = 0;
            {
                std::lock_guard<std::mutex> lock(m_frameMutex);
                renderFrame = ++m_eventsFrame;
            }
            m_frameCV.notify_one();

            // Wait until the update thread has finished producing draw commands.
            {
                std::unique_lock<std::mutex> lock(m_frameMutex);
                m_frameCV.wait(lock, [this, renderFrame] {
                    return m_commandsFrame >= renderFrame || !m_running.load();
                });

                if (!m_running) {
                    break;
                }
            }

            if (!m_minimized) {
                lifecycle->BeginProcessingFrame();
                UIManager::GetDefault()->NewFrame();
                ImGui::NewFrame();
                OnGUI();
                UIManager::GetDefault()->Render();
                lifecycle->EndProcessingFrame();
            }
        }

        UIManager::GetDefault()->Terminate();
        NotifyShutdown();
    }

	Ref<Window> Application::GetWindow() {
		return driver->GetWindow();
	}

	std::vector<std::string> Application::GetArguments()
	{
		return CommandLineArguments::GetArguments();
	}

	bool Application::OnWindowClose(WindowCloseEvent& e) {
		NotifyShutdown();
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e) {
//		VWOLF_CORE_DEBUG(e.ToString());
		m_minimized = e.GetHeight() == 0 && e.GetWidth() == 0;
        driver->Resize(e.GetWidth(), e.GetHeight());
		return false;
	}
}
