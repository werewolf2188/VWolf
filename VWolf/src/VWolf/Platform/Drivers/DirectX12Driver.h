#pragma once

#include "VWolf/Core/Driver.h"

struct DirectX12Context;

namespace VWolf {
	class DirectX12Driver : public Driver {
	public:
		virtual void Initialize(InitConfiguration config) override;
		virtual void Shutdown() override;
	private:
		void* handle;
	protected:
		DirectX12Context* context;
	};
}