#pragma once

#include "Application.h"

extern VWolf::Application* getApplication();

#define VWOLF_MAIN_APP(App) \
VWolf::Application* getApplication() {\
	return new App(); \
}

int main(int args, const char** argv) {

	VWolf::CommandLineArguments::SetArguments(args, argv);

	VWolf::Application* app = getApplication();	

	app->Run();

	delete app;
	return 0;
}