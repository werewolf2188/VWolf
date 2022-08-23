#pragma once
#define VWOLF_USE_EVENT_QUEUE 0

#if VWOLF_USE_EVENT_QUEUE
#include <map>
#include <unordered_set>
#include <functional>
#include <vector>
#include <utility>
#endif

#include <string>

namespace VWolf {
	enum class EventType
	{
		None = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		AppTick, AppUpdate, AppRender, // ?????
		KeyPressed, KeyReleased, KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};


#define EVENT_CLASS_TYPE(type) \
static EventType GetStaticType() { return EventType::type; }\
virtual EventType GetEventType() const override { return GetStaticType(); }\
virtual const char* GetName() const override { return #type; }

	class Event {
	public:
		virtual ~Event() = default;

		bool Handled = false;

		virtual EventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		//virtual int GetCategoryFlags() const = 0;
		virtual std::string ToString() const { return GetName(); }

		//bool IsInCategory(EventCategory category)
		//{
		//	return GetCategoryFlags() & category;
		//}
	};

	template<typename T, typename F>
	bool Dispatch(Event& evt, const F& func)
	{
		if (evt.GetEventType() == T::GetStaticType())
		{
			evt.Handled |= func(static_cast<T&>(evt));
			return true;
		}
		return false;
	}

	inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.ToString();
	}

#if VWOLF_USE_EVENT_QUEUE
	// WIP
	// Events right now will use the dispatch function to avoid dealing with
	// memory issues. Once all events are done and they are tested thoroughly,
	// then the queue will be tested. 
	// Dont want to have to instantiate things in the heap.
	class EventQueue {

	public:
		void Dispatch() {
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

		void Queue(Event* evt) {
			events.push_back(evt);
		}

		template <typename T>
		void Subscribe(std::function<bool(T&)> function) {
			auto& vector = functions[T::GetStaticType()];
			vector.push_back([function](Event* e) {
				T* evt = static_cast<T*>(e);
				return function(*evt);
			});
		}
	public:
		static EventQueue* defaultQueue;
	private:
		std::vector<Event*> events;
		std::map<EventType, std::vector<std::function<bool(Event*)>>> functions;
	};

	inline EventQueue* EventQueue::defaultQueue = new EventQueue();
#endif
}