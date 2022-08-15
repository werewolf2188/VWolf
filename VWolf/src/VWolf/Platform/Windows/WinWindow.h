#pragma once
#include "VWolf/Core/Window.h"

namespace VWolf {
	class WinWindow : public Window {
	public: // Inherits
		WinWindow(void* handle, InitConfiguration config);
		virtual ~WinWindow() override;
		virtual void Initialize() override;
		virtual void Run() override;
	public: 
		LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
		void* GetHWND() { return hwnd; };
	private:
		// TODO: Not sure if I want this here. For now I'll leave it.
		bool ProcessMessages();
	private: 
		void* hwnd = nullptr;
		void* hInstance = nullptr;
		InitConfiguration config;		
	};
}