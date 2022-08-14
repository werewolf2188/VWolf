#include <iostream>

#include "VWolf.h"
#include "VWolf/Core/EntryPoint.h"

class SandboxApplication : public VWolf::Application {
public:
	SandboxApplication(): Application(VWolf::DriverType::OpenGL) { 
	}

	~SandboxApplication() {

	}

	virtual void Run() override {
		std::cout << "Hello World!" << std::endl;
	}
};

extern VWolf::Application* getApplication() {
	return new SandboxApplication();
}