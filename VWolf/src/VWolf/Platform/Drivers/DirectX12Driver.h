#pragma once

#include "VWolf/Core/Driver.h"

struct DirectX12Context;

struct HINSTANCE__;

namespace VWolf {
	class DirectX12Driver : public Driver {
	public:
		virtual void Initialize(InitConfiguration config) override;
		virtual void Shutdown() override;
	private:
		HINSTANCE__* handle;
	protected:
		DirectX12Context* context;
	};
}