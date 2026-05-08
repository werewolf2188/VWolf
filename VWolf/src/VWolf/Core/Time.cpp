#include "vwpch.h"
#include "Time.h"

namespace VWolf
{

	Ref<Time> Time::m_instanceTime = nullptr;
	float Time::m_deltaTime = 0;
	float Time::m_lastFrame = 0;
	long long Time::m_frameCount = 0;

    class GenericTime: public Time {
    protected:
       virtual float GetTime() override;
    };

	float GenericTime::GetTime() {
		return (float)glfwGetTime();
	}

    Ref<Time> CreateGenericTime() {
        return CreateRef<GenericTime>();
    }
}
