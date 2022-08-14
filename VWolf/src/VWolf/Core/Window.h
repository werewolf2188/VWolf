#pragma once

namespace VWolf {
	class Window {
	public:
		virtual ~Window() {};
		virtual void Initialize() = 0;
	protected:
		Window() {};
	};
}