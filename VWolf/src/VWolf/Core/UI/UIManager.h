#pragma once

#include "VWolf/Core/Base.h"

namespace VWolf {
	class UIManager {
	public:
#ifdef VWOLF_CORE
		virtual void Initialize() = 0;
		virtual void Terminate() = 0;
		virtual void Render() = 0;
#endif
		virtual void NewFrame() = 0;
	public:
		static Ref<UIManager> GetDefault() { return defaultManager; }
#ifdef VWOLF_CORE
		static void SetDefault(Ref<UIManager> manager) { defaultManager = manager; }
#endif
	private:
		static Ref<UIManager> defaultManager;
	};

	inline Ref<UIManager> UIManager::defaultManager = nullptr;
}