#include "vwpch.h"
#include "Event.h"

namespace VWolf {
	Scope<EventQueue> EventQueue::DefaultQueue = CreateScope<EventQueue>();

	void EventQueue::Dispatch(std::function<bool()> shouldInterrupt) {
		std::vector<Ref<Event>> toDispatch;
		{
			std::scoped_lock lock(mtx);
			toDispatch.swap(events);
		}

		for (auto evt : toDispatch) {
			for (auto& [key, value] : functions) {
				if (key == evt->GetEventType()) {
					for (auto& function : value) {
                        if (shouldInterrupt()) break;
                        
						evt->Handled |= function(evt);
					}
				}
			}
		}
	}

	void EventQueue::Queue(Ref<Event> evt) {
		std::scoped_lock lock(mtx);
		events.push_back(evt);
	}

	void EventQueue::Subscribe(EventType type, std::function<bool(Ref<Event>)> function) {
		std::scoped_lock lock(mtx);
		auto& vector = functions[type];
		vector.push_back(function);
	}
}
