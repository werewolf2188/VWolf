#include "vwpch.h"

#ifdef VW_PLATFORM_WINDOWS
#include <Windows.h>
#include "DirectX12Driver.h"

#include "VWolf/Platform/Windows/WinWindow.h"

namespace VWolf {
	void DirectX12Driver::Initialize()
	{
		handle = GetModuleHandle(nullptr);
		window = new WinWindow(handle);
		window->Initialize();
	}

	void DirectX12Driver::Shutdown()
	{
		delete window;
	}
}
#endif