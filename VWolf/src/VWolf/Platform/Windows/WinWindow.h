#pragma once
#include "VWolf/Core/Window.h"

namespace VWolf {
	class WinWindow : public Window {
	public:
		WinWindow(void* handle, InitConfiguration config);
		virtual ~WinWindow() override;
		virtual void Initialize() override;
	private: 
		void* hwnd = nullptr;
		void* hInstance = nullptr;
		InitConfiguration config;
		// TODO: Remove
		static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		bool ProcessMessages();
	};
}