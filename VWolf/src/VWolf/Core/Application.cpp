#include "vwpch.h"

#include "Driver.h"
#include "Application.h"

namespace VWolf {
	Application::Application(DriverType type, InitConfiguration config) : driver(Driver::GetDriver(type))
	{
		driver->Initialize(config);
	}

	Application::~Application()
	{
		driver->Shutdown();
	}

	void Application::Run() {
		// TODO: Replace by events
		while (!driver->GetWindow()->ShouldClose()) {
			// TODO: Replace by graphics context
			driver->GetWindow()->Clear();
		}
	}	
}