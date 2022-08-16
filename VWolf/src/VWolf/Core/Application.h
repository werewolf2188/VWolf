#pragma once

#include "Base.h"

class Driver;

namespace VWolf {
	class Application {
	public:
		~Application();
		void Run();
	protected: 
		Application() = delete;
		Application(DriverType type, InitConfiguration config);
	private:
		Driver* driver;
	};
}