#include "vwpch.h"

#include "VWolf/Core/Events/ApplicationEvent.h"
#include "VWolf/Core/Events/MouseEvent.h"
#include "VWolf/Core/Events/KeyEvent.h"

#ifdef VW_PLATFORM_WINDOWS
#include "WinWindow.h"

const wchar_t* CLASS_NAME = L"VWOLF_MAIN_WINDOW";

// TODO: Move this in case of needing it.
const wchar_t* GetWC(const char* c)
{
    const size_t cSize = strlen(c) + 1;
    wchar_t* wc = new wchar_t[cSize];
    mbstowcs(wc, c, cSize);

    return wc;
}

namespace VWolf {
    enum class KeyState: int {
        RELEASE = 0, PRESS = 1, REPEAT = 2
    };
    static KeyMods GetKeyMods();
    static KeyCode GetKeyCodeFrom(int scancode);
    static KeyState codes[351];
    static WCHAR highSurrogate;

    namespace Private {
        LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
        {
            WinWindow* pThis;
            if (uMsg == WM_CREATE)
            {
                CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
                pThis = reinterpret_cast<WinWindow*>(pCreate->lpCreateParams);
                SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);
            }
            else
            {
                pThis = (WinWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
            }

            if (pThis)
            {
                return pThis->HandleMessage(uMsg, wParam, lParam);
            }
            else
            {
                return DefWindowProc(hwnd, uMsg, wParam, lParam);
            }
            return TRUE;
        }
    }

	WinWindow::WinWindow(HINSTANCE__* handle, InitConfiguration config, WindowEventCallback& callback): hInstance(handle), callback(callback)
	{
        memset(codes, (int)KeyState::RELEASE, 351);
        this->width = config.width;
        this->height = config.height;
        this->title = config.title;
        WNDCLASS wndClass = {};
        wndClass.style = CS_HREDRAW | CS_VREDRAW;
        wndClass.lpszClassName = CLASS_NAME;
        wndClass.hInstance = (HINSTANCE)hInstance;
        wndClass.hIcon = LoadIcon(NULL, IDI_WINLOGO);
        wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
        wndClass.lpfnWndProc = Private::WindowProc;
        wndClass.cbClsExtra = 0;
        wndClass.cbWndExtra = 0;

        RegisterClass(&wndClass);
	}

	WinWindow::~WinWindow()
	{
        UnregisterClass(CLASS_NAME, (HINSTANCE)hInstance);
	}

	void WinWindow::Initialize()
	{
        DWORD style = WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_MAXIMIZEBOX | WS_OVERLAPPEDWINDOW;

        RECT desktop;
        const HWND hDesktop = GetDesktopWindow();
        GetWindowRect(hDesktop, &desktop);

        RECT rect;
        rect.left = (desktop.right / 2) - (width / 2);
        rect.top = (desktop.bottom / 2) - (height / 2);
        rect.right = rect.left + width;
        rect.bottom = rect.top + height;

        AdjustWindowRect(&rect, style, false);
        hwnd = CreateWindowEx(
            0,
            CLASS_NAME,
            GetWC(title),
            style,
            rect.left,
            rect.top,
            rect.right - rect.left,
            rect.bottom - rect.top,
            nullptr,
            nullptr,
            (HINSTANCE)hInstance,
            this);
        ShowWindow(hwnd, SW_SHOW);
        UpdateWindow(hwnd);        
	}

    void WinWindow::Clear() {
        clearFunc();
    }

    LRESULT WinWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
        switch (uMsg) {
        case WM_CLOSE: 
            {
#if VWOLF_USE_EVENT_QUEUE
            WindowCloseEvent* evt = new WindowCloseEvent;
            EventQueue::defaultQueue->Queue(evt);
            callback.OnEvent(*evt);
#else 
            WindowCloseEvent evt;
            callback.OnEvent(evt);
#endif
            }           
            DestroyWindow(hwnd);
            break;
        case WM_SIZE:
            width = LOWORD(lParam);
            height = HIWORD(lParam);
            {
#if VWOLF_USE_EVENT_QUEUE
                WindowResizeEvent* evt = new WindowResizeEvent(width, height);
                EventQueue::defaultQueue->Queue(evt);
                callback.OnEvent(*evt);
#else
                WindowResizeEvent evt(width, height);
                callback.OnEvent(evt);
#endif
            }
            break;
        case WM_MOUSEMOVE:
            {
#if VWOLF_USE_EVENT_QUEUE
                MouseMovedEvent* evt = new MouseMovedEvent(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                EventQueue::defaultQueue->Queue(evt);
                callback.OnEvent(*evt);
#else
                MouseMovedEvent evt(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
                callback.OnEvent(evt);
#endif
            }
            break;
        case WM_MOUSEWHEEL:
        {
#if VWOLF_USE_EVENT_QUEUE
            MouseScrolledEvent* evt = new MouseScrolledEvent(0.0, (SHORT)HIWORD(wParam) / (double)WHEEL_DELTA);
            EventQueue::defaultQueue->Queue(evt);
            callback.OnEvent(*evt);
#else
            MouseScrolledEvent evt(0.0, (SHORT)HIWORD(wParam) / (double)WHEEL_DELTA);
            callback.OnEvent(evt);
#endif            
        }
        break;
        case WM_MOUSEHWHEEL:
        {
            // This message is only sent on Windows Vista and later
            // NOTE: The X-axis is inverted for consistency with macOS and X11
#if VWOLF_USE_EVENT_QUEUE
            MouseScrolledEvent* evt = new MouseScrolledEvent(-((SHORT)HIWORD(wParam) / (double)WHEEL_DELTA), 0.0);
            EventQueue::defaultQueue->Queue(evt);
            callback.OnEvent(*evt);
#else
            MouseScrolledEvent evt(-((SHORT)HIWORD(wParam) / (double)WHEEL_DELTA), 0.0);
            callback.OnEvent(evt);
#endif
        }
        break;
        case WM_LBUTTONDOWN:
            FireMouseDown(0);
            break;
        case WM_MBUTTONDOWN:
            FireMouseDown(1);
            break;
        case WM_RBUTTONDOWN:
            FireMouseDown(2);
            break;

        case WM_LBUTTONUP:
            FireMouseUp(0);
            break;
        case WM_MBUTTONUP:
            FireMouseUp(1);
            break;
        case WM_RBUTTONUP:
            FireMouseUp(2);
            break;
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYUP:
        {
            KeyCode key;
            int scancode;
            const KeyState action = (HIWORD(lParam) & KF_UP) ? KeyState::RELEASE : KeyState::PRESS;
            const KeyMods mods = GetKeyMods();

            scancode = (HIWORD(lParam) & (KF_EXTENDED | 0xff));
            if (!scancode)
            {
                // NOTE: Some synthetic key messages have a scancode of zero
                // HACK: Map the virtual key back to a usable scancode
                scancode = MapVirtualKeyW((UINT)wParam, MAPVK_VK_TO_VSC);
            }

            // HACK: Alt+PrtSc has a different scancode than just PrtSc
            if (scancode == 0x54)
                scancode = 0x137;

            // HACK: Ctrl+Pause has a different scancode than just Pause
            if (scancode == 0x146)
                scancode = 0x45;

            // HACK: CJK IME sets the extended bit for right Shift
            if (scancode == 0x136)
                scancode = 0x36;

            key = GetKeyCodeFrom(scancode);

            // The Ctrl keys require special handling
            if (wParam == VK_CONTROL)
            {
                if (HIWORD(lParam) & KF_EXTENDED)
                {
                    // Right side keys have the extended key bit set
                    key = KeyCode::RightControl;
                }
                else
                {
                    // NOTE: Alt Gr sends Left Ctrl followed by Right Alt
                    // HACK: We only want one event for Alt Gr, so if we detect
                    //       this sequence we discard this Left Ctrl message now
                    //       and later report Right Alt normally
                    MSG next;
                    const DWORD time = GetMessageTime();

                    if (PeekMessageW(&next, NULL, 0, 0, PM_NOREMOVE))
                    {
                        if (next.message == WM_KEYDOWN ||
                            next.message == WM_SYSKEYDOWN ||
                            next.message == WM_KEYUP ||
                            next.message == WM_SYSKEYUP)
                        {
                            if (next.wParam == VK_MENU &&
                                (HIWORD(next.lParam) & KF_EXTENDED) &&
                                next.time == time)
                            {
                                // Next message is Right Alt down so discard this
                                break;
                            }
                        }
                    }

                    // This is a regular Left Ctrl message
                    key = KeyCode::LeftControl;
                }
            }
            else if (wParam == VK_PROCESSKEY)
            {
                // IME notifies that keys have been filtered by setting the
                // virtual key-code to VK_PROCESSKEY
                break;
            }

            if (action == KeyState::RELEASE && wParam == VK_SHIFT)
            {
                // HACK: Release both Shift keys on Shift up event, as when both
                //       are pressed the first release does not emit any event
                // NOTE: The other half of this is in _glfwPollEventsWin32
                FireKeyEvent(KeyCode::LeftShift, scancode, action, mods);
                FireKeyEvent(KeyCode::RightShift, scancode, action, mods);
            }
            else if (wParam == VK_SNAPSHOT)
            {
                // HACK: Key down is not reported for the Print Screen key
                FireKeyEvent(key, scancode, KeyState::PRESS, mods);
                FireKeyEvent(key, scancode, KeyState::RELEASE, mods);
            }
            else
                FireKeyEvent(key, scancode, action, mods);

            break;
        }
        case WM_CHAR:
        case WM_SYSCHAR:
        {
            if (wParam >= 0xd800 && wParam <= 0xdbff)
                highSurrogate = (WCHAR)wParam;
            else
            {
                uint32_t codepoint = 0;

                if (wParam >= 0xdc00 && wParam <= 0xdfff)
                {
                    if (highSurrogate)
                    {
                        codepoint += (highSurrogate - 0xd800) << 10;
                        codepoint += (WCHAR)wParam - 0xdc00;
                        codepoint += 0x10000;
                    }
                }
                else
                    codepoint = (WCHAR)wParam;

                highSurrogate = 0;
                FireCharEvent(codepoint, GetKeyMods(), uMsg != WM_SYSCHAR);
            }

            return 0;
        }

        case WM_UNICHAR:
        {
            if (wParam == UNICODE_NOCHAR)
            {
                // WM_UNICHAR is not sent by Windows, but is sent by some
                // third-party input method engine
                // Returning TRUE here announces support for this message
                return TRUE;
            }

            FireCharEvent((uint32_t)wParam, GetKeyMods(), true);
            return 0;
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        }
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    void WinWindow::FireMouseDown(int button) {
#if VWOLF_USE_EVENT_QUEUE
        MouseButtonPressedEvent* evt = new MouseButtonPressedEvent(static_cast<MouseCode>(button));
        EventQueue::defaultQueue->Queue(evt);
        callback.OnEvent(*evt);
#else
        MouseButtonPressedEvent evt(static_cast<MouseCode>(button));
        callback.OnEvent(evt);
#endif
    }

    void WinWindow::FireMouseUp(int button) {
#if VWOLF_USE_EVENT_QUEUE
        MouseButtonReleasedEvent* evt = new MouseButtonReleasedEvent(static_cast<MouseCode>(button));
        EventQueue::defaultQueue->Queue(evt);
        callback.OnEvent(*evt);
#else
        MouseButtonReleasedEvent evt(static_cast<MouseCode>(button));
        callback.OnEvent(evt);
#endif
    }

    void WinWindow::FireCharEvent(unsigned int code, KeyMods mods, bool isSysChar) {
        if(code < 32 || (code > 126 && code < 160))
            return;

        // 
        mods &= ~(KeyMods::CAPS_LOCK | KeyMods::NUM_LOCK);

#if VWOLF_USE_EVENT_QUEUE
        KeyTypedEvent* evt = new KeyTypedEvent(boost::lexical_cast<std::string>(static_cast<unsigned char>(code)));
        EventQueue::defaultQueue->Queue(evt);
        callback.OnEvent(*evt);
#else				
        // For now I'm only using char to string
        // In the future it should wchar_t to wstring
        KeyTypedEvent evt(boost::lexical_cast<std::string>(static_cast<unsigned char>(code)));
        callback.OnEvent(evt);
#endif
    }

    void WinWindow::FireKeyEvent(KeyCode code, int scancode, KeyState state, KeyMods mods) {
        bool repeated = false;

        if (state == KeyState::RELEASE && codes[int(code)] == KeyState::RELEASE)
            return;

        if (state == KeyState::PRESS && codes[int(code)] == KeyState::PRESS)
            repeated = true;

        codes[int(code)] = state;

        mods &= ~(KeyMods::CAPS_LOCK | KeyMods::NUM_LOCK);

        if (state == KeyState::PRESS) {
#if VWOLF_USE_EVENT_QUEUE
            KeyPressedEvent* evt = new KeyPressedEvent(code, mods, repeated);
            EventQueue::defaultQueue->Queue(evt);
            callback.OnEvent(*evt);
#else
            KeyPressedEvent evt(code, mods, repeated);
            callback.OnEvent(evt);
#endif
        }
        else if (state == KeyState::RELEASE) {
#if VWOLF_USE_EVENT_QUEUE
            KeyReleasedEvent* evt = new KeyReleasedEvent(code, mods);
            EventQueue::defaultQueue->Queue(evt);
            callback.OnEvent(*evt);
#else
            KeyReleasedEvent evt(code, mods);
            callback.OnEvent(evt);
#endif
        }
    }

    void WinWindow::OnUpdate() {
        ProcessMessages();
    }

    void WinWindow::ProcessMessages() {
        MSG msg = {};
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    static KeyCode GetKeyCodeFrom(int scancode) {
        switch (scancode) {
            case 0x00B: return KeyCode::D0;
            case 0x002: return  KeyCode::D1;
            case 0x003: return  KeyCode::D2;
            case 0x004: return  KeyCode::D3;
            case 0x005: return  KeyCode::D4;
            case 0x006: return  KeyCode::D5;
            case 0x007: return  KeyCode::D6;
            case 0x008: return  KeyCode::D7;
            case 0x009: return  KeyCode::D8;
            case 0x00A: return  KeyCode::D9;
            case 0x01E: return  KeyCode::A;
            case 0x030: return  KeyCode::B;
            case 0x02E: return  KeyCode::C;
            case 0x020: return  KeyCode::D;
            case 0x012: return  KeyCode::E;
            case 0x021: return  KeyCode::F;
            case 0x022: return  KeyCode::G;
            case 0x023: return  KeyCode::H;
            case 0x017: return  KeyCode::I;
            case 0x024: return  KeyCode::J;
            case 0x025: return  KeyCode::K;
            case 0x026: return  KeyCode::L;
            case 0x032: return  KeyCode::M;
            case 0x031: return  KeyCode::N;
            case 0x018: return  KeyCode::O;
            case 0x019: return  KeyCode::P;
            case 0x010: return  KeyCode::Q;
            case 0x013: return  KeyCode::R;
            case 0x01F: return  KeyCode::S;
            case 0x014: return  KeyCode::T;
            case 0x016: return  KeyCode::U;
            case 0x02F: return  KeyCode::V;
            case 0x011: return  KeyCode::W;
            case 0x02D: return  KeyCode::X;
            case 0x015: return  KeyCode::Y;
            case 0x02C: return  KeyCode::Z;

            case 0x028: return  KeyCode::Apostrophe;
            case 0x02B: return  KeyCode::Backslash;
            case 0x033: return  KeyCode::Comma;
            case 0x00D: return  KeyCode::Equal;
            case 0x029: return  KeyCode::GraveAccent;
            case 0x01A: return  KeyCode::LeftBracket;
            case 0x00C: return  KeyCode::Minus;
            case 0x034: return  KeyCode::Period;
            case 0x01B: return  KeyCode::RightBracket;
            case 0x027: return  KeyCode::Semicolon;
            case 0x035: return  KeyCode::Slash;
            case 0x056: return  KeyCode::World2;

            case 0x00E: return  KeyCode::Backspace;
            case 0x153: return  KeyCode::Delete;
            case 0x14F: return  KeyCode::End;
            case 0x01C: return  KeyCode::Enter;
            case 0x001: return  KeyCode::Escape;
            case 0x147: return  KeyCode::Home;
            case 0x152: return  KeyCode::Insert;
            case 0x15D: return  KeyCode::Menu;
            case 0x151: return  KeyCode::PageDown;
            case 0x149: return  KeyCode::PageUp;
            case 0x045: return  KeyCode::Pause;
            case 0x039: return  KeyCode::Space;
            case 0x00F: return  KeyCode::Tab;
            case 0x03A: return  KeyCode::CapsLock;
            case 0x145: return  KeyCode::NumLock;
            case 0x046: return  KeyCode::ScrollLock;
            case 0x03B: return  KeyCode::F1;
            case 0x03C: return  KeyCode::F2;
            case 0x03D: return  KeyCode::F3;
            case 0x03E: return  KeyCode::F4;
            case 0x03F: return  KeyCode::F5;
            case 0x040: return  KeyCode::F6;
            case 0x041: return  KeyCode::F7;
            case 0x042: return  KeyCode::F8;
            case 0x043: return  KeyCode::F9;
            case 0x044: return  KeyCode::F10;
            case 0x057: return  KeyCode::F11;
            case 0x058: return  KeyCode::F12;
            case 0x064: return  KeyCode::F13;
            case 0x065: return  KeyCode::F14;
            case 0x066: return  KeyCode::F15;
            case 0x067: return  KeyCode::F16;
            case 0x068: return  KeyCode::F17;
            case 0x069: return  KeyCode::F18;
            case 0x06A: return  KeyCode::F19;
            case 0x06B: return  KeyCode::F20;
            case 0x06C: return  KeyCode::F21;
            case 0x06D: return  KeyCode::F22;
            case 0x06E: return  KeyCode::F23;
            case 0x076: return  KeyCode::F24;
            case 0x038: return  KeyCode::LeftAlt;
            case 0x01D: return  KeyCode::LeftControl;
            case 0x02A: return  KeyCode::LeftShift;
            case 0x15B: return  KeyCode::LeftSuper;
            case 0x137: return  KeyCode::PrintScreen;
            case 0x138: return  KeyCode::RightAlt;
            case 0x11D: return  KeyCode::RightControl;
            case 0x036: return  KeyCode::RightShift;
            case 0x15C: return  KeyCode::RightShift;
            case 0x150: return  KeyCode::Down;
            case 0x14B: return  KeyCode::Left;
            case 0x14D: return  KeyCode::Right;
            case 0x148: return  KeyCode::Up;

            case 0x052: return  KeyCode::KP0;
            case 0x04F: return  KeyCode::KP1;
            case 0x050: return  KeyCode::KP2;
            case 0x051: return  KeyCode::KP3;
            case 0x04B: return  KeyCode::KP4;
            case 0x04C: return  KeyCode::KP5;
            case 0x04D: return  KeyCode::KP6;
            case 0x047: return  KeyCode::KP7;
            case 0x048: return  KeyCode::KP8;
            case 0x049: return  KeyCode::KP9;
            case 0x04E: return  KeyCode::KPAdd;
            case 0x053: return  KeyCode::KPDecimal;
            case 0x135: return  KeyCode::KPDivide;
            case 0x11C: return  KeyCode::KPEnter;
            case 0x059: return  KeyCode::KPEqual;
            case 0x037: return  KeyCode::KPMultiply;
            case 0x04A: return  KeyCode::KPSubtract;
        }
        return KeyCode::Unknown;
    }

    static KeyMods GetKeyMods()
    {
        KeyMods mods = KeyMods::NONE;

        if (GetKeyState(VK_SHIFT) & 0x8000)
            mods |= KeyMods::SHIFT;
        if (GetKeyState(VK_CONTROL) & 0x8000)
            mods |= KeyMods::CONTROL;
        if (GetKeyState(VK_MENU) & 0x8000)
            mods |= KeyMods::ALT;
        if ((GetKeyState(VK_LWIN) | GetKeyState(VK_RWIN)) & 0x8000)
            mods |= KeyMods::SUPER;
        if (GetKeyState(VK_CAPITAL) & 1)
            mods |= KeyMods::CAPS_LOCK;
        if (GetKeyState(VK_NUMLOCK) & 1)
            mods |= KeyMods::NUM_LOCK;

        return mods;
    }
}

#endif