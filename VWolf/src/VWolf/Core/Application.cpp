#include "vwpch.h"

#include "Driver.h"
#include "Application.h"

#include "Log.h"

namespace VWolf {
	Application::Application(DriverType type, InitConfiguration config) : driver(Driver::GetDriver(type))
	{
		VWOLF_CORE_INFO("Initializing core application");
		driver->Initialize(config);
	}

	Application::~Application()
	{
		VWOLF_CORE_INFO("Shutting down core application");
		driver->Shutdown();
	}

	void Application::Run() {
		VWOLF_CORE_INFO("Running core application");
		// TODO: Replace by events
		while (!driver->GetWindow()->ShouldClose()) {
			// TODO: Replace by graphics context
			VWOLF_CORE_INFO("Clearing window");
			driver->GetWindow()->Clear();
		}
	}	
}