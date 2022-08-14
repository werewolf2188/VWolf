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
		
	}
	
}