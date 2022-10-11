#pragma once

#include "VWolf/Core/Driver.h"

namespace VWolf { 
	class OpenGLDriver : public Driver, public WindowEventCallback {
	public:
        virtual ~OpenGLDriver() override {};
		virtual void Initialize(InitConfiguration config, WindowEventCallback& callback) override;
		virtual void Shutdown() override;
		virtual void OnUpdate() override;
		virtual void OnEvent(Event& evt) override;
        virtual void Resize(unsigned int m_Width, unsigned int m_Height) override;
	private:
		WindowEventCallback* callback;
	};
}