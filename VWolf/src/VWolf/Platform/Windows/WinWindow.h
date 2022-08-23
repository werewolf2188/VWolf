#pragma once
#include "VWolf/Core/Window.h"

struct HWND__;
struct HINSTANCE__;

namespace VWolf {
	enum class KeyCode;
	enum class KeyMods;
	enum class KeyState;

	class WinWindow : public Window {
	public: // Inherits
		WinWindow(HINSTANCE__* handle, InitConfiguration config, WindowEventCallback& callback);
		virtual ~WinWindow() override;
		virtual void Initialize() override;
		virtual void OnUpdate() override;
		//TODO: Remove
		virtual void Clear() override;
	public: 
		LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
		HWND__* GetHWND() { return hwnd; };		
		// TODO: Remove
		std::function<void()> clearFunc;
	private:
		void ProcessMessages();
		void FireMouseDown(int button);
		void FireMouseUp(int button);
		void FireKeyEvent(KeyCode code, int scancode, KeyState state, KeyMods mods);
		void FireCharEvent(unsigned int code, KeyMods mods, bool isSysChar);
	private: 
		HWND__* hwnd = nullptr;
		HINSTANCE__* hInstance = nullptr;
		const char* title;
		WindowEventCallback& callback;
	};
}