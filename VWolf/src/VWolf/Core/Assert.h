#pragma once

#include "Base.h"
#include "Log.h"
#include <filesystem>

#ifdef VWOLF_ENABLE_ASSERTS

	// Alteratively we could use the same "default" message for both "WITH_MSG" and "NO_MSG" and
	// provide support for custom formatting by concatenating the formatting string instead of having the format inside the default message
	#define VWOLF_INTERNAL_ASSERT_IMPL(type, check, msg, ...) { if(!(check)) { VWOLF##type##ERROR(msg, __VA_ARGS__); VWOLF_DEBUGBREAK(); } }
	#define VWOLF_INTERNAL_ASSERT_WITH_MSG(type, check, ...) VWOLF_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: %s", __VA_ARGS__)
	#define VWOLF_INTERNAL_ASSERT_NO_MSG(type, check) VWOLF_INTERNAL_ASSERT_IMPL(type, check, "Assertion '%s' failed at %s:%d", VWOLF_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string().c_str(), __LINE__)

	#define VWOLF_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
	#define VWOLF_INTERNAL_ASSERT_GET_MACRO(...) VWOLF_EXPAND_MACRO( VWOLF_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, VWOLF_INTERNAL_ASSERT_WITH_MSG, VWOLF_INTERNAL_ASSERT_NO_MSG) )

	// Currently accepts at least the condition and one additional parameter (the message) being optional
	#define VWOLF_CLIENT_ASSERT(...) VWOLF_EXPAND_MACRO( VWOLF_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CLIENT_, __VA_ARGS__) )
	#define VWOLF_CORE_ASSERT(...) VWOLF_EXPAND_MACRO( VWOLF_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__) )
#else
	#define VWOLF_CLIENT_ASSERT(...)
	#define VWOLF_CORE_ASSERT(...)
#endif