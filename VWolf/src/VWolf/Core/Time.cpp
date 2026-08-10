#include "vwpch.h"
#include "Time.h"

#include "VWolf/Core/Events/Event.h"
#include "VWolf/Core/Events/ApplicationEvent.h"

namespace VWolf
{
    Scope<Time> Time::m_instanceTime = nullptr;
	float Time::m_deltaTime = 0;
	float Time::m_lastFrame = 0;
	long long Time::m_frameCount = 0;

    class GenericTime: public Time {
    protected:
       virtual float GetTime() override;
       virtual void SendAppTick() override;
    };

	float GenericTime::GetTime() {
		return (float)glfwGetTime();
	}

    void GenericTime::SendAppTick() {
        EventQueue::DefaultQueue->Queue(CreateRef<AppTickEvent>());
    }

    Scope<Time> CreateGenericTime() {
        return CreateScope<GenericTime>();
    }
}
