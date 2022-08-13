#pragma once

#include "Application.h"

extern VWolf::Application* getApplication();

int main(int args, const char* argv) {

	VWolf::Application* app = getApplication();

	app->Run();

	delete app;
	return 0;
}