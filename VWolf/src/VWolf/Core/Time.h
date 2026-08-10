#pragma once

#include "Base.h"

#include <chrono>

namespace VWolf {
	class Time {
	public:
		static float GetDeltaTime() {
			return m_deltaTime;
		}

		static float GetFramesPerSecond() {
			return 1 / GetDeltaTime();
		}

		static float GetTotalTime() {
			return m_lastFrame;
		}

		static long long GetTotalFrames() {
			return m_frameCount;
		}

#ifdef VWOLF_CORE
		static void Tick() {
			float time = m_instanceTime->GetTime();
			m_deltaTime = time - m_lastFrame;
			m_lastFrame = time;
			m_frameCount += 1;
            m_instanceTime->SendAppTick();
		}

		static void SetTimeImplementation(Scope<Time> instance) { m_instanceTime = std::move(instance); }
#endif
	protected:
        virtual float GetTime() = 0;
        virtual void SendAppTick() = 0;
	private:
		static Scope<Time> m_instanceTime;
		static float m_deltaTime;
		static float m_lastFrame;
		static long long m_frameCount;
	};

    Scope<Time> CreateGenericTime();
}
