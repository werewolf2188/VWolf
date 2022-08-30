#pragma once

namespace VWolf {

#define VWOLF_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

	struct InitConfiguration {
		int width;
		int height;
		const char* title;
	};

	enum class DriverType {
#ifdef VW_PLATFORM_WINDOWS
		DirectX12,
#endif
		OpenGL
	};

	inline const char* DriverName(DriverType type) {
		switch (type) {
#ifdef VW_PLATFORM_WINDOWS
		case DriverType::DirectX12: return "DirectX 12";
#endif
		case DriverType::OpenGL: return "OpenGL";
		}
		return "";
	}
}