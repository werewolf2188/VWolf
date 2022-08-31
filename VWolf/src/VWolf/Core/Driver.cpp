#include "vwpch.h"

#include "Driver.h"

#include "VWolf/Platform/Drivers/OpenGLDriver.h"
#ifdef VWOLF_PLATFORM_WINDOWS
#include "VWolf/Platform/Drivers/DirectX12Driver.h"
#endif

namespace VWolf {
	Scope<Driver> Driver::GetDriver(DriverType type) {
		switch (type) {
#ifdef VWOLF_PLATFORM_WINDOWS
		case DriverType::DirectX12: return CreateScope<DirectX12Driver>();
#endif
		case DriverType::OpenGL: return  CreateScope<OpenGLDriver>();
		default: return CreateScope<OpenGLDriver>();
		}		
	}
}