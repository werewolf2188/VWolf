#pragma once

#include "Base.h"
#include "Window.h"

namespace VWolf {
	class Driver {
	public:
		virtual void Initialize(InitConfiguration config) = 0;
		virtual void Shutdown() = 0;
		Window* GetWindow() { return window; };

		static Driver* GetDriver(DriverType type);
	protected:
		Window* window;
	};
}