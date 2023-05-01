#pragma once

#include "PlatformDetection.h"

#include <memory>

namespace VWolf {

#define VWOLF_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

#ifdef DEBUG
	#if defined(VWOLF_PLATFORM_WINDOWS)
		#define VWOLF_DEBUGBREAK() __debugbreak()
	#elif defined(VWOLF_PLATFORM_LINUX)
		#include <signal.h>
		#define VWOLF_DEBUGBREAK() raise(SIGTRAP)
	#else
		//#error "Platform doesn't support debugbreak yet!" // TODO: Not sure if I want to cause a compiling error
		#define VWOLF_DEBUGBREAK()
	#endif
	#define VWOLF_ENABLE_ASSERTS
#else
	#define VWOLF_DEBUGBREAK()
#endif

#define VWOLF_EXPAND_MACRO(x) x
#define VWOLF_STRINGIFY_MACRO(x) #x

	struct InitConfiguration {
		int width;
		int height;
		const char* title;
		bool maximize = false;
	};

	enum class DriverType {
#ifdef VWOLF_PLATFORM_WINDOWS
		DirectX12,
#endif
		OpenGL
	};

	inline const char* DriverName(DriverType type) {
		switch (type) {
#ifdef VWOLF_PLATFORM_WINDOWS
		case DriverType::DirectX12: return "DirectX 12";
#endif
		case DriverType::OpenGL: return "OpenGL";
		}
		return "Null Driver. Please use a driver already implemented.";
	}

	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

    template<typename T>
    constexpr Ref<T> CreateFromRef(T& t)
    {
        return std::make_shared<T>(std::forward<T>(t));
    }
}

#include "Assert.h"
#include "Utils/File.h"