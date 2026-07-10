#pragma once

#include "VWolf/Core/Driver.h"

namespace VWolf { 
	class OpenGLDriver : public Driver, public EventCallback {
	public:
        virtual ~OpenGLDriver() override {};
		virtual void Initialize(InitConfiguration config, EventCallback& callback) override;
		virtual void Shutdown() override;
		virtual void OnUpdate() override;
		virtual void OnEvent(Event& evt) override;
        virtual void Resize(unsigned int m_Width, unsigned int m_Height) override;
	public:
		static OpenGLDriver* GetCurrentDriver() { return currentDriver; }
	private:
		EventCallback* callback;

		static OpenGLDriver* currentDriver;
	};
}
