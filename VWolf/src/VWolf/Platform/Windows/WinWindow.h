#pragma once
#include "VWolf/Core/Window.h"
#include "VWolf/Core/Events/MouseEvent.h"
#include "VWolf/Core/Events/KeyEvent.h"

struct HWND__;
struct HINSTANCE__;

namespace VWolf {
	enum class KeyCode;
	enum class KeyMods;
	enum class KeyState;

	class WinWindow : public Window, public MouseHandler, public KeyHandler {
	public: // Inherits
		WinWindow(HINSTANCE__* handle, InitConfiguration config, WindowEventCallback& callback);
		virtual ~WinWindow() override;
		virtual void Initialize() override;
		virtual void OnUpdate() override;
		virtual bool IsMouseButtonPressed(MouseCode button) override;
		virtual std::pair<float, float> GetMousePosition() override;
		virtual bool IsKeyPressed(KeyCode key) override;
		virtual void* GetNativeWindow() override { return hwnd; };
	public: 
		LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
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
		bool maximize = false;
		WindowEventCallback& callback;
	};
}