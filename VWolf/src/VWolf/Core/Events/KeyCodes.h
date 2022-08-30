#pragma once

#include "Event.h"

namespace VWolf {
	enum class KeyCode {
		Space,
		Apostrophe, /* ' */
		Comma, /* , */
		Minus, /* - */
		Period, /* . */
		Slash, /* / */

		D0, /* 0 */
		D1, /* 1 */
		D2, /* 2 */
		D3, /* 3 */
		D4, /* 4 */
		D5, /* 5 */
		D6, /* 6 */
		D7, /* 7 */
		D8, /* 8 */
		D9, /* 9 */

		Semicolon, /* ; */
		Equal, /* = */

		A,
		B,
		C,
		D,
		E,
		F,
		G,
		H,
		I,
		J,
		K,
		L,
		M,
		N,
		O,
		P,
		Q,
		R,
		S,
		T,
		U,
		V,
		W,
		X,
		Y,
		Z,

		LeftBracket,  /* [ */
		Backslash,  /* \ */
		RightBracket,  /* ] */
		GraveAccent,  /* ` */

		World1, /* non-US #1 */
		World2, /* non-US #2 */

		/* Function keys */
		Escape,
		Enter,
		Tab,
		Backspace,
		Insert,
		Delete,
		Right,
		Left,
		Down,
		Up,
		PageUp,
		PageDown,
		Home,
		End,
		CapsLock,
		ScrollLock,
		NumLock,
		PrintScreen,
		Pause,
		F1,
		F2,
		F3,
		F4,
		F5,
		F6,
		F7,
		F8,
		F9,
		F10,
		F11,
		F12,
		F13,
		F14,
		F15,
		F16,
		F17,
		F18,
		F19,
		F20,
		F21,
		F22,
		F23,
		F24,
		F25,

		/* Keypad */
		KP0,
		KP1,
		KP2,
		KP3,
		KP4,
		KP5,
		KP6,
		KP7,
		KP8,
		KP9,
		KPDecimal,
		KPDivide,
		KPMultiply,
		KPSubtract,
		KPAdd,
		KPEnter,
		KPEqual,

		LeftShift,
		LeftControl,
		LeftAlt,
		LeftSuper,
		RightShift,
		RightControl,
		RightAlt,
		RightSuper,
		Menu,
		Unknown
	};

	inline std::ostream& operator<<(std::ostream& os, const KeyCode& e)
	{
		switch (e) {
		case KeyCode::Space: os << " (space)"; break;
		case KeyCode::Apostrophe: os << " '(apostrophe) "; break;
		case KeyCode::Comma: os << " ,(comma) "; break;
		case KeyCode::Minus: os << " -(minus) "; break;
		case KeyCode::Period: os << " .(period) "; break;
		case KeyCode::Slash: os << " /(slash) "; break;

		case KeyCode::D0: os << " 0(D0) "; break;
		case KeyCode::D1: os << " 1(D1) "; break;
		case KeyCode::D2: os << " 2(D2) "; break;
		case KeyCode::D3: os << " 3(D3) "; break;
		case KeyCode::D4: os << " 4(D4) "; break;
		case KeyCode::D5: os << " 5(D5) "; break;
		case KeyCode::D6: os << " 6(D6) "; break;
		case KeyCode::D7: os << " 7(D7) "; break;
		case KeyCode::D8: os << " 8(D8) "; break;
		case KeyCode::D9: os << " 9(D9) "; break;

		case KeyCode::Semicolon: os << " ;(semicolon) "; break;
		case KeyCode::Equal: os << " =(equal) "; break;

		case KeyCode::A: os << " A "; break;
		case KeyCode::B: os << " B "; break;
		case KeyCode::C: os << " C "; break;
		case KeyCode::D: os << " D "; break;
		case KeyCode::E: os << " E "; break;
		case KeyCode::F: os << " F "; break;
		case KeyCode::G: os << " G "; break;
		case KeyCode::H: os << " H "; break;
		case KeyCode::I: os << " I "; break;
		case KeyCode::J: os << " J "; break;
		case KeyCode::K: os << " K "; break;
		case KeyCode::L: os << " L "; break;
		case KeyCode::M: os << " M "; break;
		case KeyCode::N: os << " N "; break;
		case KeyCode::O: os << " O "; break;
		case KeyCode::P: os << " P "; break;
		case KeyCode::Q: os << " Q "; break;
		case KeyCode::R: os << " R "; break;
		case KeyCode::S: os << " S "; break;
		case KeyCode::T: os << " T "; break;
		case KeyCode::U: os << " U "; break;
		case KeyCode::V: os << " V "; break;
		case KeyCode::W: os << " W "; break;
		case KeyCode::X: os << " X "; break;
		case KeyCode::Y: os << " Y "; break;
		case KeyCode::Z: os << " Z "; break;

		case KeyCode::LeftBracket: os << " [(LeftBracket) "; break;
		case KeyCode::Backslash: os << " \\(Backslash) "; break;
		case KeyCode::RightBracket: os << " ](RightBracket) "; break;
		case KeyCode::GraveAccent: os << " `(GraveAccent) "; break;

		case KeyCode::World1: os << " ( non-US #1) "; break;
		case KeyCode::World2: os << " ( non-US #2) "; break;

		case KeyCode::Escape: os << " (Escape) "; break;
		case KeyCode::Enter: os << " (Enter) "; break;
		case KeyCode::Tab: os << " (Tab) "; break;
		case KeyCode::Backspace: os << " (Backspace) "; break;
		case KeyCode::Insert: os << " (Insert) "; break;
		case KeyCode::Delete: os << " (Delete) "; break;
		case KeyCode::Right: os << " (Right) "; break;
		case KeyCode::Left: os << " (Left) "; break;
		case KeyCode::Down: os << " (Down) "; break;
		case KeyCode::Up: os << " (Up) "; break;
		case KeyCode::PageUp: os << " (PageUp) "; break;
		case KeyCode::PageDown: os << " (PageDown) "; break;
		case KeyCode::Home: os << " (Home) "; break;
		case KeyCode::End: os << " (End) "; break;
		case KeyCode::CapsLock: os << " (CapsLock) "; break;
		case KeyCode::ScrollLock: os << " (ScrollLock) "; break;
		case KeyCode::NumLock: os << " (NumLock) "; break;
		case KeyCode::PrintScreen: os << " (PrintScreen) "; break;
		case KeyCode::Pause: os << " (Pause) "; break;

		case KeyCode::F1: os << " (F1) "; break;
		case KeyCode::F2: os << " (F2) "; break;
		case KeyCode::F3: os << " (F3) "; break;
		case KeyCode::F4: os << " (F4) "; break;
		case KeyCode::F5: os << " (F5) "; break;
		case KeyCode::F6: os << " (F6) "; break;
		case KeyCode::F7: os << " (F7) "; break;
		case KeyCode::F8: os << " (F8) "; break;
		case KeyCode::F9: os << " (F9) "; break;
		case KeyCode::F10: os << " (F10) "; break;
		case KeyCode::F11: os << " (F11) "; break;
		case KeyCode::F12: os << " (F12) "; break;
		case KeyCode::F13: os << " (F13) "; break;
		case KeyCode::F14: os << " (F14) "; break;
		case KeyCode::F15: os << " (F15) "; break;
		case KeyCode::F16: os << " (F16) "; break;
		case KeyCode::F17: os << " (F17) "; break;
		case KeyCode::F18: os << " (F18) "; break;
		case KeyCode::F19: os << " (F19) "; break;
		case KeyCode::F20: os << " (F20) "; break;
		case KeyCode::F21: os << " (F21) "; break;
		case KeyCode::F22: os << " (F22) "; break;
		case KeyCode::F23: os << " (F23) "; break;
		case KeyCode::F24: os << " (F24) "; break;
		case KeyCode::F25: os << " (F25) "; break;

		case KeyCode::KP0: os << " 0(KP0) "; break;
		case KeyCode::KP1: os << " 1(KP1) "; break;
		case KeyCode::KP2: os << " 2(KP2) "; break;
		case KeyCode::KP3: os << " 3(KP3) "; break;
		case KeyCode::KP4: os << " 4(KP4) "; break;
		case KeyCode::KP5: os << " 5(KP5) "; break;
		case KeyCode::KP6: os << " 6(KP6) "; break;
		case KeyCode::KP7: os << " 7(KP7) "; break;
		case KeyCode::KP8: os << " 8(KP8) "; break;
		case KeyCode::KP9: os << " 9(KP9) "; break;
		case KeyCode::KPDecimal: os << " .(KPDecimal) "; break;
		case KeyCode::KPDivide: os << " /(KPDivide) "; break;
		case KeyCode::KPMultiply: os << " *(KPMultiply) "; break;
		case KeyCode::KPSubtract: os << " -(KPSubtract) "; break;
		case KeyCode::KPAdd: os << " +(KPAdd) "; break;
		case KeyCode::KPEnter: os << " (KPEnter) "; break;
		case KeyCode::KPEqual: os << " =(KPEqual) "; break;

		case KeyCode::LeftShift: os << " (LeftShift) "; break;
		case KeyCode::LeftControl: os << " (LeftControl) "; break;
		case KeyCode::LeftAlt: os << " (LeftAlt) "; break;
		case KeyCode::LeftSuper: os << " (LeftSuper) "; break;
		case KeyCode::RightShift: os << " (RightShift) "; break;
		case KeyCode::RightControl: os << " (RightControl) "; break;
		case KeyCode::RightAlt: os << " (RightAlt) "; break;
		case KeyCode::RightSuper: os << " (RightSuper) "; break;
		case KeyCode::Menu: os << " (Menu) "; break;
		case KeyCode::Unknown: os << " (Unknown) "; break;
		}
		return os;
	}

	enum class KeyMods: int {
		NONE = 0,
		SHIFT = 0x0001,
		CONTROL = 0x0002,
		ALT = 0x0004,
		SUPER = 0x0008,
		CAPS_LOCK = 0x0010,
		NUM_LOCK = 0x0020
	};

	inline constexpr KeyMods operator&(KeyMods x, KeyMods y)
	{
		return static_cast<KeyMods> (static_cast<int>(x) & static_cast<int>(y));
	}

	inline constexpr KeyMods operator|(KeyMods x, KeyMods y)
	{
		return static_cast<KeyMods> (static_cast<int>(x) | static_cast<int>(y));
	}

	inline constexpr KeyMods operator^(KeyMods x, KeyMods y)
	{
		return static_cast<KeyMods> (static_cast<int>(x) ^ static_cast<int>(y));
	}

	inline constexpr KeyMods operator~(KeyMods x)
	{
		return static_cast<KeyMods>(~static_cast<int>(x));
	}

	inline KeyMods& operator&=(KeyMods& x, KeyMods y)
	{
		x = x & y;
		return x;
	}

	inline KeyMods& operator|=(KeyMods& x, KeyMods y)
	{
		x = x | y;
		return x;
	}

	inline KeyMods& operator^=(KeyMods& x, KeyMods y)
	{
		x = x ^ y;
		return x;
	}

	inline std::ostream& operator<<(std::ostream& os, const KeyMods& e)
	{
		os << "Mods:";
		if (static_cast<int>(e) & static_cast<int>(KeyMods::SHIFT))
			os << " SHIFT ";
		if (static_cast<int>(e) & static_cast<int>(KeyMods::CONTROL))
			os << " CONTROL ";
		if (static_cast<int>(e) & static_cast<int>(KeyMods::ALT))
			os << " ALT ";
		if (static_cast<int>(e) & static_cast<int>(KeyMods::SUPER))
			os << " SUPER ";
		if (static_cast<int>(e) & static_cast<int>(KeyMods::CAPS_LOCK))
			os << " CAPS_LOCK ";
		if (static_cast<int>(e) & static_cast<int>(KeyMods::NUM_LOCK))
			os << " NUM_LOCK ";
		return os;
	}
}