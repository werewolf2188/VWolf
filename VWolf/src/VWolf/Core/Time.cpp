#include "vwpch.h"
#include "Time.h"

namespace VWolf
{
	Ref<Time> Time::m_instanceTime = nullptr;
	float Time::m_deltaTime = 0;
	float Time::m_lastFrame = 0;
	long long Time::m_frameCount = 0;

	float GenericTimer::GetTime() {
		return (float)glfwGetTime();
	}
}