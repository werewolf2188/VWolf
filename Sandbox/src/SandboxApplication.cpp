#include <iostream>

#include "VWolf.h"
#include "VWolf/Core/EntryPoint.h"

class SandboxApplication : public VWolf::Application {
public:
	SandboxApplication(): Application(VWolf::DriverType::OpenGL) { }

	virtual void Run() override {
		std::cout << "Hello World!" << std::endl;
	}
};

VWOLF_MAIN_APP(SandboxApplication)