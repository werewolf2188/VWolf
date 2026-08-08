#pragma once

#include "Application.h"

// https://learn.microsoft.com/en-us/windows/win32/learnwin32/winmain--the-application-entry-point
// https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-winmain
// https://www.youtube.com/watch?v=Hqw0barvNVk&t=4s

extern VWolf::Scope<VWolf::Application> getApplication();

#define VWOLF_MAIN_APP(App) \
VWolf::Scope<VWolf::Application> getApplication() {\
	return VWolf::CreateScope<App>(); \
}

#if defined(VWOLF_PLATFORM_MACOS) || defined(VWOLF_PLATFORM_IOS)
    #define NS_PRIVATE_IMPLEMENTATION
    #define MTL_PRIVATE_IMPLEMENTATION
    #define MTK_PRIVATE_IMPLEMENTATION
    #define CA_PRIVATE_IMPLEMENTATION
    #include <MetalKit/MetalKit.hpp>
    #include <AppKit/AppKit.hpp>
    #include <Metal/Metal.hpp>
#endif

int main(int args, const char** argv) {

	VWolf::CommandLineArguments::SetArguments(args, argv);

    VWolf::Application::SetApplication(getApplication());

    VWolf::Application::GetApplication()->Initialize();
    
    VWolf::Application::GetApplication()->Run();
    
    VWolf::Application::GetApplication()->Shutdown();

	return 0;
}
