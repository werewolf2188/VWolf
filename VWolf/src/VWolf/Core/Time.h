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

		static float GetTotalFrames() {
			return m_frameCount;
		}

#ifdef VWOLF_CORE
		static void Tick() {
			float time = m_instanceTime->GetTime();
			m_deltaTime = time - m_lastFrame;
			m_lastFrame = time;
			m_frameCount += 1;
		}

		static void SetTimeImplementation(Ref<Time> instance) { m_instanceTime = instance; }
#endif
	protected:
		virtual float GetTime() = 0;		
	private:
		static Ref<Time> m_instanceTime;
		static float m_deltaTime;
		static float m_lastFrame;
		static long long m_frameCount;
	};

	inline Ref<Time> Time::m_instanceTime = nullptr;
	inline float Time::m_deltaTime = 0;
	inline float Time::m_lastFrame = 0;
	inline long long Time::m_frameCount = 0;
}