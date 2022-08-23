#pragma once

#include "Event.h"
#include "KeyCodes.h"

#include <sstream>

namespace VWolf {
	
	class KeyEvent : public Event
	{
	public:
		KeyCode GetKeyCode() const { return m_KeyCode; }

		// EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
	protected:
		KeyEvent(const KeyCode keycode, const KeyMods keyMods)
			: m_KeyCode(keycode), m_KeyMods(keyMods) {}

		KeyCode m_KeyCode;
		KeyMods m_KeyMods;
	};

	class KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(const KeyCode keycode, const KeyMods keyMods, bool isRepeat = false)
			: KeyEvent(keycode, keyMods), m_IsRepeat(isRepeat) {}

		bool IsRepeat() const { return m_IsRepeat; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: " << m_KeyCode << " (repeat = " << m_IsRepeat << ") " << m_KeyMods;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyPressed)
	private:
		bool m_IsRepeat;
	};

	class KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(const KeyCode keycode, const KeyMods keyMods)
			: KeyEvent(keycode, keyMods) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << m_KeyCode << m_KeyMods;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyReleased)
	};

	class KeyTypedEvent : public Event
	{
	public:
		KeyTypedEvent(std::string keycode): m_KeyCode(keycode){}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyTypedEvent: " << m_KeyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyTyped)
	private:
		std::string m_KeyCode;
	};
}