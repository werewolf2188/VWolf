#pragma once

#include "VWolf/Core/Base.h"
#include "VWolf/Core/Events/Event.h"

namespace VWolf {
	class UIManager {
	public:
		virtual ~UIManager() = default;
#ifdef VWOLF_CORE
		virtual void Initialize() = 0;
		virtual void Terminate() = 0;
		virtual void Render() = 0;
#endif
		virtual void NewFrame() = 0;
		virtual bool OnEvent(Event& evt) = 0;
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