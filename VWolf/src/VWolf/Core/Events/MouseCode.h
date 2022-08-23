#pragma once
#include "Event.h"

namespace VWolf {
	enum class MouseCode {
		Left, Middle, Right, Unknown
	};

	inline std::ostream& operator<<(std::ostream& os, const MouseCode& e)
	{
		std::string value = "";
		switch (e) {
		case MouseCode::Left: value = "Left"; break;
		case MouseCode::Middle: value = "Middle"; break;
		case MouseCode::Right: value = "Right"; break;
		}
		return os << value;
	}
}