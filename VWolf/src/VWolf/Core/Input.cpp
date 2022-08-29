#include "vwpch.h"

#include "Base.h"
#include "Input.h"
#include "Application.h"

#include "VWolf/Core/Events/MouseEvent.h"
#include "VWolf/Core/Events/KeyEvent.h"

namespace VWolf {

	MouseHandler* GetMouseHandler() {
		return dynamic_cast<MouseHandler*>(Application::GetApplication()->GetWindow());
	}

	KeyHandler* GetKeyHandler() {
		return dynamic_cast<KeyHandler*>(Application::GetApplication()->GetWindow());
	}

	bool Input::IsKeyPressed(KeyCode key) {
		return GetKeyHandler()->IsKeyPressed(key);
	}

	bool Input::IsMouseButtonPressed(MouseCode button) {
		return GetMouseHandler()->IsMouseButtonPressed(button);
	}

	std::pair<float, float> Input::GetMousePosition() {
		return GetMouseHandler()->GetMousePosition();
	}

	float Input::GetMouseX() {
		return GetMousePosition().first;
	}

	float Input::GetMouseY() {
		return GetMousePosition().second;
	}
}