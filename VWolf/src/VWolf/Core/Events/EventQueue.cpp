#include "vwpch.h"
#include "Event.h"

namespace VWolf {
	Scope<EventQueue> EventQueue::DefaultQueue = CreateScope<EventQueue>();

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

	void EventQueue::Queue(Ref<Event> evt) {
		events.push_back(evt);
	}

	void EventQueue::Subscribe(EventType type, std::function<bool(Ref<Event>)> function) {
		auto& vector = functions[type];
		vector.push_back(function);
	}
}
