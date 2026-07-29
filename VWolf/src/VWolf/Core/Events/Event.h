#pragma once

#include <map>
#include <functional>
#include <vector>
#include <string>

#include "VWolf/Core/Base.h"

namespace VWolf {
	enum class EventType
	{
		None = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved, WindowDrapDrop,
		AppTick, AppUpdate, AppRender, // ?????
		KeyPressed, KeyReleased, KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled,
        CustomEvent
	};


#define EVENT_CLASS_TYPE(type) \
static EventType GetStaticType() { return EventType::type; }\
virtual EventType GetEventType() const override { return GetStaticType(); }\
virtual const char* GetName() const override { return #type; }

#define EVENT_CUSTOM_CLASS(name) \
static VWolf::EventType GetStaticType() { return VWolf::EventType::CustomEvent; }\
virtual VWolf::EventType GetEventType() const override { return GetStaticType(); }\
virtual const char* GetName() const override { return name; }

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

	inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.ToString();
	}

	// WIP
	/*
	* For now the dispatcher function will do since there's an issue with the window resize event
	* using the queue. Also, the queue is still working in the same thread. It will cool 
	* to make it work in a separate thread to allow the main thread to rest.
	* Probably hide the implementation in the unit of compilation (file)
	*/
	class EventQueue {

	public:
		void Dispatch();
		void Queue(Ref<Event> evt);
		void Subscribe(EventType type, std::function<bool(Ref<Event>)> function);

		template <typename T>
		void Subscribe(std::function<bool(T&)> function) {
			EventType type = T::GetStaticType();
			auto functionWrapper = [function](Ref<Event> e) {
                Ref<T> evt = std::dynamic_pointer_cast<T>(e);
				return function(*evt);
			};
			Subscribe(type, functionWrapper);
		}
	public:
		static Scope<EventQueue> DefaultQueue;
	private:
		std::vector<Ref<Event>> events;
		std::map<EventType, std::vector<std::function<bool(Ref<Event>)>>> functions;
	};
}
