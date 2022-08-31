#pragma once

#include "Base.h"
#include "Window.h"

namespace VWolf {
	class Driver {
	public:
		virtual void Initialize(InitConfiguration config, WindowEventCallback& callback) = 0;
		virtual void Shutdown() = 0;
		virtual void OnUpdate() = 0;
		Ref<Window> GetWindow() { return window; };

		static Scope<Driver> GetDriver(DriverType type);
	protected:
		Ref<Window> window;
	};
}