#pragma once

#include "Base.h"

namespace VWolf {
	class Driver {
	public:
		virtual void Initialize() = 0;
		virtual void Shutdown() = 0;

		static Driver* GetDriver(DriverType type);
	};
}