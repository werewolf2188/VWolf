#include "vwpch.h"

#include "Driver.h"
#include "Application.h"

namespace VWolf {
	void Application::Run() {
		
	}
	Application::Application(DriverType type): driver(Driver::GetDriver(type))
	{
	}
}