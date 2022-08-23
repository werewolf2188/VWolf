#include "vwpch.h"
#include "GLFWWindow.h"

#include "VWolf/Core/Events/ApplicationEvent.h"
#include "VWolf/Core/Events/MouseEvent.h"
#include "VWolf/Core/Events/KeyEvent.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace VWolf {
	static KeyCode GetKeyCodeFrom(int key);
	static KeyMods GetKeyModsFrom(int mods);

	MouseCode GetMouseCode(int button) {
		switch (button) {
		case 0: return MouseCode::Left;
		case 1: return MouseCode::Right;
		case 2: return MouseCode::Middle;
		}
		return MouseCode::Unknown;
	}

	GLFWWindow::GLFWWindow(InitConfiguration config, WindowEventCallback& callback): Window(), callback(callback)
	{
		this->width = config.width;
		this->height = config.height;
		
		m_window = glfwCreateWindow(width, height, config.title, NULL, NULL);
		if (m_window == NULL)
		{
			std::cout << "Failed to create GLFW window" << std::endl;
			WindowCloseEvent evt;
			GetCallback().OnEvent(evt);
			glfwTerminate();
		}
		glfwMakeContextCurrent(m_window);
		glfwSetWindowUserPointer(m_window, this);

		// Setting events
		glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window) {
			GLFWWindow& data = *(GLFWWindow*)glfwGetWindowUserPointer(window);
#if VWOLF_USE_EVENT_QUEUE
			WindowCloseEvent* evt = new WindowCloseEvent;
			EventQueue::defaultQueue->Queue(evt);
			data.GetCallback().OnEvent(*evt);
#else 
			WindowCloseEvent evt;
			data.GetCallback().OnEvent(evt);
#endif
		});

		glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int width, int height)
		{
			GLFWWindow& data = *(GLFWWindow*)glfwGetWindowUserPointer(window);
			data.SetWidth(width);
			data.SetHeight(height);
#if VWOLF_USE_EVENT_QUEUE
			WindowResizeEvent* evt = new WindowResizeEvent(width, height);
			EventQueue::defaultQueue->Queue(evt);
			data.GetCallback().OnEvent(*evt);
#else
			WindowResizeEvent evt(width, height);
			data.GetCallback().OnEvent(evt);
#endif
		});

		glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double xPos, double yPos)
		{
			GLFWWindow& data = *(GLFWWindow*)glfwGetWindowUserPointer(window);
#if VWOLF_USE_EVENT_QUEUE
			MouseMovedEvent* evt = new MouseMovedEvent(xPos, yPos);
			EventQueue::defaultQueue->Queue(evt);
			data.GetCallback().OnEvent(*evt);
#else
			MouseMovedEvent evt(xPos, yPos);
			data.GetCallback().OnEvent(evt);
#endif
		});

		glfwSetScrollCallback(m_window, [](GLFWwindow* window, double xOffset, double yOffset) {
			GLFWWindow& data = *(GLFWWindow*)glfwGetWindowUserPointer(window);
#if VWOLF_USE_EVENT_QUEUE
			MouseScrolledEvent* evt = new MouseScrolledEvent(xOffset, yOffset);
			EventQueue::defaultQueue->Queue(evt);
			data.GetCallback().OnEvent(*evt);
#else
			MouseScrolledEvent evt(xOffset, yOffset);
			data.GetCallback().OnEvent(evt);
#endif
		});

		glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods)
		{
			GLFWWindow& data = *(GLFWWindow*)glfwGetWindowUserPointer(window);

			switch (action) {
				case GLFW_PRESS:
				{
#if VWOLF_USE_EVENT_QUEUE
					MouseButtonPressedEvent* evt = new MouseButtonPressedEvent(GetMouseCode(button));
					EventQueue::defaultQueue->Queue(evt);
					data.GetCallback().OnEvent(*evt);
#else
					MouseButtonPressedEvent evt(GetMouseCode(button));
					data.GetCallback().OnEvent(evt);
#endif
				}
				break;
				case GLFW_RELEASE:
				{
#if VWOLF_USE_EVENT_QUEUE
					MouseButtonReleasedEvent* evt = new MouseButtonReleasedEvent(GetMouseCode(button));
					EventQueue::defaultQueue->Queue(evt);
					data.GetCallback().OnEvent(*evt);
#else
					MouseButtonReleasedEvent evt(GetMouseCode(button));
					data.GetCallback().OnEvent(evt);
#endif				
				}
				break;
			}
		});

		glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			GLFWWindow& data = *(GLFWWindow*)glfwGetWindowUserPointer(window);

			switch (action)
			{
			case GLFW_PRESS:
				{
	#if VWOLF_USE_EVENT_QUEUE
					KeyPressedEvent* evt = new KeyPressedEvent(GetKeyCodeFrom(key), GetKeyModsFrom(mods), 0);
					EventQueue::defaultQueue->Queue(evt);
					data.GetCallback().OnEvent(*evt);
	#else
					KeyPressedEvent evt(GetKeyCodeFrom(key), GetKeyModsFrom(mods), 0);
					data.GetCallback().OnEvent(evt);
	#endif
					break;
				}
			case GLFW_RELEASE:
				{
	#if VWOLF_USE_EVENT_QUEUE
					KeyReleasedEvent* evt = new KeyReleasedEvent(GetKeyCodeFrom(key), GetKeyModsFrom(mods));
					EventQueue::defaultQueue->Queue(evt);
					data.GetCallback().OnEvent(*evt);
	#else
					KeyReleasedEvent evt(GetKeyCodeFrom(key), GetKeyModsFrom(mods));
					data.GetCallback().OnEvent(evt);
	#endif
					break;
				}
			case GLFW_REPEAT:
				{
	#if VWOLF_USE_EVENT_QUEUE
					KeyPressedEvent* evt = new KeyPressedEvent(GetKeyCodeFrom(key), GetKeyModsFrom(mods), true);
					EventQueue::defaultQueue->Queue(evt);
					data.GetCallback().OnEvent(*evt);
	#else
					KeyPressedEvent evt(GetKeyCodeFrom(key), GetKeyModsFrom(mods), true);
					data.GetCallback().OnEvent(evt);
	#endif
					break;
				}
			}
		});

		glfwSetCharCallback(m_window, [](GLFWwindow* window, unsigned int keycode)
		{
				GLFWWindow& data = *(GLFWWindow*)glfwGetWindowUserPointer(window);
#if VWOLF_USE_EVENT_QUEUE
				KeyTypedEvent* evt = new KeyTypedEvent(boost::lexical_cast<std::string>(static_cast<unsigned char>(keycode)));
				EventQueue::defaultQueue->Queue(evt);
				data.GetCallback().OnEvent(*evt);
#else				
				// For now I'm only using char to string
				// In the future it should wchar_t to wstring
				KeyTypedEvent evt(boost::lexical_cast<std::string>(static_cast<unsigned char>(keycode)));
				data.GetCallback().OnEvent(evt);
#endif
		});
	}

	GLFWWindow::~GLFWWindow()
	{
		glfwDestroyWindow(m_window);
	}

	void GLFWWindow::Initialize()
	{
		// TODO: Move
		glViewport(0, 0, width, height);
		// TODO: Initialize UI		
	}

	void GLFWWindow::Clear() {
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);			
	}

	void GLFWWindow::OnUpdate() {
		glfwPollEvents();		
	}

	static KeyCode GetKeyCodeFrom(int key) {
		
		switch (key) {
		case GLFW_KEY_SPACE: return KeyCode::Space;
		case GLFW_KEY_APOSTROPHE: return KeyCode::Apostrophe;
		case GLFW_KEY_COMMA: return KeyCode::Comma;
		case GLFW_KEY_MINUS: return KeyCode::Minus;
		case GLFW_KEY_PERIOD: return KeyCode::Period;
		case GLFW_KEY_SLASH: return KeyCode::Slash;

		case GLFW_KEY_0: return KeyCode::D0;
		case GLFW_KEY_1: return KeyCode::D1;
		case GLFW_KEY_2: return KeyCode::D2;
		case GLFW_KEY_3: return KeyCode::D3;
		case GLFW_KEY_4: return KeyCode::D4;
		case GLFW_KEY_5: return KeyCode::D5;
		case GLFW_KEY_6: return KeyCode::D6;
		case GLFW_KEY_7: return KeyCode::D7;
		case GLFW_KEY_8: return KeyCode::D8;
		case GLFW_KEY_9: return KeyCode::D9;

		case GLFW_KEY_SEMICOLON: return KeyCode::Semicolon;
		case GLFW_KEY_EQUAL: return KeyCode::Equal;

		case GLFW_KEY_A: return KeyCode::A;
		case GLFW_KEY_B: return KeyCode::B;
		case GLFW_KEY_C: return KeyCode::C;
		case GLFW_KEY_D: return KeyCode::D;
		case GLFW_KEY_E: return KeyCode::E;
		case GLFW_KEY_F: return KeyCode::F;
		case GLFW_KEY_G: return KeyCode::G;
		case GLFW_KEY_H: return KeyCode::H;
		case GLFW_KEY_I: return KeyCode::I;
		case GLFW_KEY_J: return KeyCode::J;
		case GLFW_KEY_K: return KeyCode::K;
		case GLFW_KEY_L: return KeyCode::L;
		case GLFW_KEY_M: return KeyCode::M;
		case GLFW_KEY_N: return KeyCode::N;
		case GLFW_KEY_O: return KeyCode::O;
		case GLFW_KEY_P: return KeyCode::P;
		case GLFW_KEY_Q: return KeyCode::Q;
		case GLFW_KEY_R: return KeyCode::R;
		case GLFW_KEY_S: return KeyCode::S;
		case GLFW_KEY_T: return KeyCode::T;
		case GLFW_KEY_U: return KeyCode::U;
		case GLFW_KEY_V: return KeyCode::V;
		case GLFW_KEY_W: return KeyCode::W;
		case GLFW_KEY_X: return KeyCode::X;
		case GLFW_KEY_Y: return KeyCode::Y;
		case GLFW_KEY_Z: return KeyCode::Z;

		case GLFW_KEY_LEFT_BRACKET: return KeyCode::LeftBracket;
		case GLFW_KEY_BACKSLASH: return KeyCode::Backslash;
		case GLFW_KEY_RIGHT_BRACKET: return KeyCode::RightBracket;
		case GLFW_KEY_GRAVE_ACCENT: return KeyCode::GraveAccent;
		case GLFW_KEY_WORLD_1: return KeyCode::World1;
		case GLFW_KEY_WORLD_2: return KeyCode::World2;

		case GLFW_KEY_ESCAPE: return KeyCode::Escape;
		case GLFW_KEY_ENTER: return KeyCode::Enter;
		case GLFW_KEY_TAB: return KeyCode::Tab;
		case GLFW_KEY_BACKSPACE: return KeyCode::Backspace;
		case GLFW_KEY_INSERT: return KeyCode::Insert;
		case GLFW_KEY_DELETE: return KeyCode::Delete;
		case GLFW_KEY_RIGHT: return KeyCode::Right;
		case GLFW_KEY_LEFT: return KeyCode::Left;
		case GLFW_KEY_DOWN: return KeyCode::Down;
		case GLFW_KEY_UP: return KeyCode::Up;
		case GLFW_KEY_PAGE_UP: return KeyCode::PageUp;
		case GLFW_KEY_PAGE_DOWN: return KeyCode::PageDown;
		case GLFW_KEY_HOME: return KeyCode::Home;
		case GLFW_KEY_END: return KeyCode::End;
		case GLFW_KEY_CAPS_LOCK: return KeyCode::CapsLock;
		case GLFW_KEY_SCROLL_LOCK: return KeyCode::ScrollLock;
		case GLFW_KEY_NUM_LOCK: return KeyCode::NumLock;
		case GLFW_KEY_PRINT_SCREEN: return KeyCode::PrintScreen;
		case GLFW_KEY_PAUSE: return KeyCode::Pause;

		case GLFW_KEY_F1: return KeyCode::F1;
		case GLFW_KEY_F2: return KeyCode::F2;
		case GLFW_KEY_F3: return KeyCode::F3;
		case GLFW_KEY_F4: return KeyCode::F4;
		case GLFW_KEY_F5: return KeyCode::F5;
		case GLFW_KEY_F6: return KeyCode::F6;
		case GLFW_KEY_F7: return KeyCode::F7;
		case GLFW_KEY_F8: return KeyCode::F8;
		case GLFW_KEY_F9: return KeyCode::F9;
		case GLFW_KEY_F10: return KeyCode::F10;
		case GLFW_KEY_F11: return KeyCode::F11;
		case GLFW_KEY_F12: return KeyCode::F12;
		case GLFW_KEY_F13: return KeyCode::F13;
		case GLFW_KEY_F14: return KeyCode::F14;
		case GLFW_KEY_F15: return KeyCode::F15;
		case GLFW_KEY_F16: return KeyCode::F16;
		case GLFW_KEY_F17: return KeyCode::F17;
		case GLFW_KEY_F18: return KeyCode::F18;
		case GLFW_KEY_F19: return KeyCode::F19;
		case GLFW_KEY_F20: return KeyCode::F20;
		case GLFW_KEY_F21: return KeyCode::F21;
		case GLFW_KEY_F22: return KeyCode::F22;
		case GLFW_KEY_F23: return KeyCode::F23;
		case GLFW_KEY_F24: return KeyCode::F24;
		case GLFW_KEY_F25: return KeyCode::F25;

		case GLFW_KEY_KP_0: return KeyCode::KP0;
		case GLFW_KEY_KP_1: return KeyCode::KP1;
		case GLFW_KEY_KP_2: return KeyCode::KP2;
		case GLFW_KEY_KP_3: return KeyCode::KP3;
		case GLFW_KEY_KP_4: return KeyCode::KP4;
		case GLFW_KEY_KP_5: return KeyCode::KP5;
		case GLFW_KEY_KP_6: return KeyCode::KP6;
		case GLFW_KEY_KP_7: return KeyCode::KP7;
		case GLFW_KEY_KP_8: return KeyCode::KP8;
		case GLFW_KEY_KP_9: return KeyCode::KP9;
		case GLFW_KEY_KP_DECIMAL: return KeyCode::KPDecimal;
		case GLFW_KEY_KP_DIVIDE: return KeyCode::KPDivide;
		case GLFW_KEY_KP_MULTIPLY: return KeyCode::KPMultiply;
		case GLFW_KEY_KP_SUBTRACT: return KeyCode::KPSubtract;
		case GLFW_KEY_KP_ADD: return KeyCode::KPAdd;
		case GLFW_KEY_KP_ENTER: return KeyCode::KPEnter;
		case GLFW_KEY_KP_EQUAL: return KeyCode::KPEqual;

		case GLFW_KEY_LEFT_SHIFT: return KeyCode::LeftShift;
		case GLFW_KEY_LEFT_CONTROL: return KeyCode::LeftControl;
		case GLFW_KEY_LEFT_ALT: return KeyCode::LeftAlt;
		case GLFW_KEY_LEFT_SUPER: return KeyCode::LeftSuper;
		case GLFW_KEY_RIGHT_SHIFT: return KeyCode::RightShift;
		case GLFW_KEY_RIGHT_CONTROL: return KeyCode::RightControl;
		case GLFW_KEY_RIGHT_ALT: return KeyCode::RightAlt;
		case GLFW_KEY_RIGHT_SUPER: return KeyCode::RightSuper;
		case GLFW_KEY_MENU: return KeyCode::Menu;
		}
		return KeyCode::Unknown;
	}

	static KeyMods GetKeyModsFrom(int mods) {
		KeyMods m = KeyMods::NONE;
		if (mods & GLFW_MOD_SHIFT)
			m |= KeyMods::SHIFT;
		if (mods & GLFW_MOD_CONTROL)
			m |= KeyMods::CONTROL;
		if (mods & GLFW_MOD_ALT)
			m |= KeyMods::ALT;
		if (mods & GLFW_MOD_SUPER)
			m |= KeyMods::SUPER;
		if (mods & GLFW_MOD_CAPS_LOCK)
			m |= KeyMods::CAPS_LOCK;
		if (mods & GLFW_MOD_NUM_LOCK)
			m |= KeyMods::NUM_LOCK;
		return m;
	}
}