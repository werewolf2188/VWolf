#include "vwpch.h"

#ifdef VW_PLATFORM_WINDOWS
#include <Windows.h>
#include "DirectX12Driver.h"

#include "VWolf/Platform/Windows/WinWindow.h"

namespace VWolf {
	void DirectX12Driver::Initialize(InitConfiguration config)
	{
		handle = GetModuleHandle(nullptr);
		window = new WinWindow(handle, config);
		window->Initialize();
	}

	void DirectX12Driver::Shutdown()
	{
		delete window;
	}
}
#endif