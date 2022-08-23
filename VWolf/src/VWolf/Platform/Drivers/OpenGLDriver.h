#pragma once

#include "VWolf/Core/Driver.h"

namespace VWolf { 
	class OpenGLDriver : public Driver, public WindowEventCallback {
	public:
		virtual void Initialize(InitConfiguration config, WindowEventCallback& callback) override;
		virtual void Shutdown() override;
		virtual void OnUpdate() override;
		virtual void OnEvent(Event& evt) override;
	private:
		WindowEventCallback* callback;
	};
}