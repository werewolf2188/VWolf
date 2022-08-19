#pragma once
#include "VWolf/Core/Window.h"

struct HWND__;
struct HINSTANCE__;

namespace VWolf {
	class WinWindow : public Window {
	public: // Inherits
		WinWindow(HINSTANCE__* handle, InitConfiguration config);
		virtual ~WinWindow() override;
		virtual void Initialize() override;
		//TODO: Remove
		virtual bool ShouldClose() override;
		virtual void Clear() override;
	public: 
		LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
		HWND__* GetHWND() { return hwnd; };
		// TODO: Remove
		std::function<void()> clearFunc;
	private:
		// TODO: Not sure if I want this here. For now I'll leave it.
		bool ProcessMessages();
	private: 
		HWND__* hwnd = nullptr;
		HINSTANCE__* hInstance = nullptr;
		InitConfiguration config;	

	};
}