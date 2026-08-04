#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>

#include "Base.h"

#include "Window.h"
#include "Events/ApplicationEvent.h"

#include "Audio/Core.h"

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

    class Lifecycle;

	class Application {
	public:
		static Ref<Application>& GetApplication() { return m_application; };
	public:
		virtual ~Application();
		virtual void OnUpdate() = 0;
        virtual void OnDraw() = 0;
		virtual void OnGUI() = 0;
	public:
		void Run();
        void Quit();
		Ref<Window> GetWindow();
		std::vector<std::string> GetArguments();
        DriverType GetDriverType() { return m_type; }
    public:
        static bool IsPlaying() { return _isPlaying; }
        static void SetPlaying(bool isPlaying) { _isPlaying = isPlaying; }
		Scope<Driver>& GetDriver() { return driver; }
	protected: 
		Application() = delete;
		Application(DriverType type, InitConfiguration config);	
		bool OnWindowResize(WindowResizeEvent& e);
	private:
		bool OnWindowClose(WindowCloseEvent& e);
        void Update();
        void Render();
        void NotifyShutdown();
    private:
        static bool _isPlaying;
	private:
		static Ref<Application> m_application;
		DriverType m_type;
		Scope<Driver> driver;
        Scope<Lifecycle> lifecycle;
		std::atomic<bool> m_running = false;
        std::atomic<bool> m_minimized = false;

        // Producer-consumer frame barrier:
        // main polls events (bumps m_eventsFrame) -> update produces commands
        // (bumps m_commandsFrame) -> main renders that frame
        std::mutex m_frameMutex;
        std::condition_variable m_frameCV;
        uint64_t m_eventsFrame = 0;
        uint64_t m_commandsFrame = 0;

        std::thread updateThread;
	};
}
