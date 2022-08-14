#pragma once

#include "Application.h"

extern VWolf::Application* getApplication();

#define VWOLF_MAIN_APP(App) \
extern VWolf::Application* getApplication() {\
	return new App(); \
}

int main(int args, const char* argv) {

	VWolf::Application* app = getApplication();

	app->Run();

	delete app;
	return 0;
}