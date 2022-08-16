#pragma once

#include "Base.h"

namespace VWolf {
	class Window {
	public:
		virtual ~Window() {};
		virtual void Initialize() = 0;
		// TODO: Remove
		virtual bool ShouldClose() = 0;
		virtual void Clear() = 0;
	protected:
		Window() {};
	};
}