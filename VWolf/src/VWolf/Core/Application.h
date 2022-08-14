#pragma once

#include "Base.h"

class Driver;

namespace VWolf {
	class Application {
	public:
		~Application();
		virtual void Run();
	protected: 
		Application() = delete;
		Application(DriverType type);
	private:
		Driver* driver;
	};
}