#pragma once

#include "VWolf/Core/Events/KeyCodes.h"
#include "VWolf/Core/Events/MouseCode.h"

namespace VWolf {
	class Input
	{
	public:
		static bool IsKeyPressed(KeyCode key);

		static bool IsMouseButtonPressed(MouseCode button);
		static std::pair<float, float> GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();
	};
}