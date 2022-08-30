#pragma once
#define VWOLF_USE_EVENT_QUEUE 0

#if VWOLF_USE_EVENT_QUEUE
#include <map>
#include <functional>
#include <vector>
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
	/*
	* For now the dispatcher function will do since there's an issue with the window resize event
	* using the queue. Also, the queue is still working in the same thread. It will cool 
	* to make it work in a separate thread to allow the main thread to rest.
	* Probably hide the implementation in the unit of compilation (file)
	*/
	class EventQueue {

	public:
		void Dispatch();
		void Queue(Event* evt);
		void Subscribe(EventType type, std::function<bool(Event*)> function);

		template <typename T>
		void Subscribe(std::function<bool(T&)> function) {
			EventType type = T::GetStaticType();
			auto functionWrapper = [function](Event* e) {
				T* evt = static_cast<T*>(e);
				return function(*evt);
			};	
			Subscribe(type, functionWrapper);
		}
	public:
		static EventQueue* defaultQueue;
	private:
		std::vector<Event*> events;
		std::map<EventType, std::vector<std::function<bool(Event*)>>> functions;
	};
#endif
}