#pragma once

#include "VWolf/Core/Driver.h"

struct DirectX12Context;

struct HINSTANCE__;

namespace VWolf {
	class DirectX12Driver : public Driver, public WindowEventCallback {
	public:
        virtual ~DirectX12Driver() override {};
		virtual void Initialize(InitConfiguration config, WindowEventCallback& callback) override;
		virtual void Shutdown() override;
		virtual void OnUpdate() override;
		virtual void OnEvent(Event& evt) override;
        virtual void Resize(unsigned int m_Width, unsigned int m_Height) override;
	private:
		HINSTANCE__* handle;
		WindowEventCallback* callback;
	protected:
		DirectX12Context* context;		
	};
}