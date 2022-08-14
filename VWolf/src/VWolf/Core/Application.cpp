#include "vwpch.h"

#include "Driver.h"
#include "Application.h"

namespace VWolf {
	Application::Application(DriverType type) : driver(Driver::GetDriver(type))
	{
		driver->Initialize();
	}

	Application::~Application()
	{
		driver->Shutdown();
	}

	void Application::Run() {
		
	}
	
}