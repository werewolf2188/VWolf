#include "vwpch.h"

#include "Driver.h"

#include "VWolf/Platform/Drivers/OpenGLDriver.h"
#ifdef VW_PLATFORM_WINDOWS
#include "VWolf/Platform/Drivers/DirectX12Driver.h"
#endif

namespace VWolf {
	Driver* Driver::GetDriver(DriverType type) {
		switch (type) {
#ifdef VW_PLATFORM_WINDOWS
		case DriverType::DirectX12: return new DirectX12Driver();
#endif
		case DriverType::OpenGL: return new OpenGLDriver();
		default: return new OpenGLDriver();
		}		
	}
}