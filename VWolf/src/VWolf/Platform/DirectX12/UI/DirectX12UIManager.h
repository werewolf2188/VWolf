#pragma once

#ifdef VWOLF_PLATFORM_WINDOWS
#include "VWolf/Core/UI/UIManager.h"

#include "VWolf/Platform/DirectX12/Core/DX12Resources.h"

namespace VWolf {

	class DirectX12UIManager : public UIManager {
	public:
		DirectX12UIManager();
		virtual ~DirectX12UIManager();
		virtual void Initialize() override;
		virtual void Terminate() override;
		virtual void Render() override;
		virtual void NewFrame() override;
		virtual bool OnEvent(Event& evt) override;
	};
}
#endif