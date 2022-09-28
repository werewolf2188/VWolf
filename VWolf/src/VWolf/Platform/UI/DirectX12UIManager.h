#pragma once

#ifdef VWOLF_PLATFORM_WINDOWS
#include "VWolf/Core/UI/UIManager.h"

struct HWND__;
struct HINSTANCE__;

struct DirectX12Context;

namespace VWolf {

	struct UIContext;

	class DirectX12UIManager : public UIManager {
	public:
		DirectX12UIManager(HWND__* window, DirectX12Context* context);
		virtual ~DirectX12UIManager();
		virtual void Initialize() override;
		virtual void Terminate() override;
		virtual void Render() override;
		virtual void NewFrame() override;
		virtual bool OnEvent(Event& evt) override;
	public:
		LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	private:
		HWND__* m_window;
		DirectX12Context* context;
		UIContext* m_uiContext;
	};
}
#endif