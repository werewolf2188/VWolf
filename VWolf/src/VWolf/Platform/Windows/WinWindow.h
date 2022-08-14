#pragma once
#include "VWolf/Core/Window.h"

namespace VWolf {
	class WinWindow : public Window {
	public:
		WinWindow(void* handle);
		virtual ~WinWindow() override;
		virtual void Initialize() override;
	private: 
		void* hwnd = nullptr;
		void* hInstance = nullptr;
		// TODO: Remove
		int width = 800;
		int height = 600;
		static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		bool ProcessMessages();
	};
}