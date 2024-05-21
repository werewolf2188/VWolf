#pragma once

#include "Application.h"

extern VWolf::Application* getApplication();

#define VWOLF_MAIN_APP(App) \
VWolf::Application* getApplication() {\
	return new App(); \
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

	VWolf::Application* app = getApplication();	

	app->Run();

	delete app;
	return 0;
}