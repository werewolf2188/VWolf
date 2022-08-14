#pragma once

#include "VWolf/Core/Driver.h"

namespace VWolf { 
	class OpenGLDriver : public Driver {
	public:
		virtual void Initialize(InitConfiguration config) override;
		virtual void Shutdown() override;
	};
}