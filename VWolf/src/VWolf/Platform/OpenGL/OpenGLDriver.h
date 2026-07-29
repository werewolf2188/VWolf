#pragma once

#include "VWolf/Core/Driver.h"

namespace VWolf { 
	class OpenGLDriver : public Driver {
	public:
        virtual ~OpenGLDriver() override {};
		virtual void Initialize(InitConfiguration config) override;
		virtual void Shutdown() override;
		virtual void OnUpdate() override;
        virtual void Resize(unsigned int m_Width, unsigned int m_Height) override;
	public:
		static OpenGLDriver* GetCurrentDriver() { return currentDriver; }

		static OpenGLDriver* currentDriver;
	};
}
