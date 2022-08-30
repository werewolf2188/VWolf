#include "vwpch.h"

#include "Event.h"

#if VWOLF_USE_EVENT_QUEUE
namespace VWolf {
	EventQueue* EventQueue::defaultQueue = new EventQueue();

	void EventQueue::Dispatch() {
		for (auto evt : events) {
			for (auto& [key, value] : functions) {
				if (key == evt->GetEventType()) {
					for (auto& function : value) {
						evt->Handled |= function(evt);
					}
				}
			}
		}
		events.clear();
	}

	void EventQueue::Queue(Event* evt) {
		events.push_back(evt);
	}

	void EventQueue::Subscribe(EventType type, std::function<bool(Event*)> function) {
		auto& vector = functions[type];
		vector.push_back(function);
	}
}
#endif