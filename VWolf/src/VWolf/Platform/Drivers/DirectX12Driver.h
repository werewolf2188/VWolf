#pragma once

#include "VWolf/Core/Driver.h"

namespace VWolf {
	class DirectX12Driver : public Driver {
	public:
		virtual void Initialize() override;
		virtual void Shutdown() override;
	};
}