//
//  Window.cpp
//  VWolf
//
//  Created by Enrique Moises on 5/7/26.
//

#include "vwpch.h"
#include "Window.h"

struct GLFWwindow;
#if defined(VWOLF_PLATFORM_MACOS) || defined(VWOLF_PLATFORM_IOS)
namespace NS {
    class Window;
    class View;
}
#elif defined(VWOLF_PLATFORM_WINDOWS)
struct HWND__;
#endif

#if defined(VWOLF_PLATFORM_MACOS) || defined(VWOLF_PLATFORM_IOS)
#define GLFW_EXPOSE_NATIVE_COCOA
#elif defined(VWOLF_PLATFORM_WINDOWS)
#define GLFW_EXPOSE_NATIVE_WIN32
#endif
#include <GLFW/glfw3native.h>

namespace VWolf {

    int GetMouseFrom(MouseCode button) {
        switch (button) {
        case MouseCode::Left: return 0;
        case MouseCode::Right: return 1;
        case MouseCode::Middle: return 2;
        default: break;
        }
        return -1;
    }

    MouseCode GetMouseCode(int button) {
        switch (button) {
        case 0: return MouseCode::Left;
        case 1: return MouseCode::Right;
        case 2: return MouseCode::Middle;
        default: break;
        }
        return MouseCode::Unknown;
    }

    int GetKeyFrom(KeyCode key) {
        switch (key) {
        case KeyCode::Space: return GLFW_KEY_SPACE;
        case KeyCode::Apostrophe: GLFW_KEY_APOSTROPHE;
        case KeyCode::Comma: return GLFW_KEY_COMMA;
        case KeyCode::Minus: GLFW_KEY_MINUS;
        case KeyCode::Period: return GLFW_KEY_PERIOD;
        case KeyCode::Slash: return GLFW_KEY_SLASH;

        case KeyCode::D0: return GLFW_KEY_0;
        case KeyCode::D1: return GLFW_KEY_1;
        case KeyCode::D2: return GLFW_KEY_2;
        case KeyCode::D3: return GLFW_KEY_3;
        case KeyCode::D4: return GLFW_KEY_4;
        case KeyCode::D5: return GLFW_KEY_5;
        case KeyCode::D6: return GLFW_KEY_6;
        case KeyCode::D7: return GLFW_KEY_7;
        case KeyCode::D8: return GLFW_KEY_8;
        case KeyCode::D9: return GLFW_KEY_9;

        case  KeyCode::Semicolon: return GLFW_KEY_SEMICOLON;
        case KeyCode::Equal: return GLFW_KEY_EQUAL;

        case KeyCode::A: return GLFW_KEY_A;
        case KeyCode::B: return GLFW_KEY_B;
        case KeyCode::C: return GLFW_KEY_C;
        case KeyCode::D: return GLFW_KEY_D;
        case KeyCode::E: return GLFW_KEY_E;
        case KeyCode::F: return GLFW_KEY_F;
        case KeyCode::G: return GLFW_KEY_G;
        case KeyCode::H: return GLFW_KEY_H;
        case KeyCode::I: return GLFW_KEY_I;
        case KeyCode::J: return GLFW_KEY_J;
        case KeyCode::K: return GLFW_KEY_K;
        case KeyCode::L: return GLFW_KEY_L;
        case KeyCode::M: return GLFW_KEY_M;
        case KeyCode::N: return GLFW_KEY_N;
        case KeyCode::O: return GLFW_KEY_O;
        case KeyCode::P: return GLFW_KEY_P;
        case KeyCode::Q: return GLFW_KEY_Q;
        case KeyCode::R: return GLFW_KEY_R;
        case KeyCode::S: return GLFW_KEY_S;
        case KeyCode::T: return GLFW_KEY_T;
        case KeyCode::U: return GLFW_KEY_U;
        case KeyCode::V: return GLFW_KEY_V;
        case KeyCode::W: return GLFW_KEY_W;
        case KeyCode::X: return GLFW_KEY_X;
        case KeyCode::Y: return GLFW_KEY_Y;
        case KeyCode::Z: return GLFW_KEY_Z;

        case KeyCode::LeftBracket: return GLFW_KEY_LEFT_BRACKET;
        case KeyCode::Backslash: return GLFW_KEY_BACKSLASH;
        case KeyCode::RightBracket: return GLFW_KEY_RIGHT_BRACKET;
        case KeyCode::GraveAccent: return GLFW_KEY_GRAVE_ACCENT;
        case KeyCode::World1: return GLFW_KEY_WORLD_1;
        case KeyCode::World2: return GLFW_KEY_WORLD_2;

        case KeyCode::Escape: return GLFW_KEY_ESCAPE;
        case KeyCode::Enter:return GLFW_KEY_ENTER;
        case KeyCode::Tab: return GLFW_KEY_TAB;
        case KeyCode::Backspace: return GLFW_KEY_BACKSPACE;
        case KeyCode::Insert: return GLFW_KEY_INSERT;
        case KeyCode::Delete: return GLFW_KEY_DELETE;
        case KeyCode::Right: return GLFW_KEY_RIGHT;
        case KeyCode::Left: return GLFW_KEY_LEFT;
        case KeyCode::Down: return GLFW_KEY_DOWN;
        case KeyCode::Up: return GLFW_KEY_UP;
        case KeyCode::PageUp: return GLFW_KEY_PAGE_UP;
        case KeyCode::PageDown: return GLFW_KEY_PAGE_DOWN;
        case KeyCode::Home: return GLFW_KEY_HOME;
        case KeyCode::End: return GLFW_KEY_END;
        case KeyCode::CapsLock: return GLFW_KEY_END;
        case KeyCode::ScrollLock: return GLFW_KEY_SCROLL_LOCK;
        case KeyCode::NumLock: return GLFW_KEY_NUM_LOCK;
        case KeyCode::PrintScreen: return GLFW_KEY_PRINT_SCREEN;
        case KeyCode::Pause: return GLFW_KEY_PAUSE;

        case KeyCode::F1: return GLFW_KEY_F1;
        case KeyCode::F2: return GLFW_KEY_F2;
        case KeyCode::F3: return GLFW_KEY_F3;
        case KeyCode::F4: return GLFW_KEY_F4;
        case KeyCode::F5: return GLFW_KEY_F5;
        case KeyCode::F6: return GLFW_KEY_F6;
        case KeyCode::F7: return GLFW_KEY_F7;
        case KeyCode::F8: return GLFW_KEY_F8;
        case KeyCode::F9: return GLFW_KEY_F9;
        case KeyCode::F10: return GLFW_KEY_F10;
        case KeyCode::F11: return GLFW_KEY_F11;
        case KeyCode::F12: return GLFW_KEY_F12;
        case KeyCode::F13: return GLFW_KEY_F13;
        case KeyCode::F14: return GLFW_KEY_F14;
        case KeyCode::F15: return GLFW_KEY_F15;
        case KeyCode::F16: return GLFW_KEY_F16;
        case KeyCode::F17: return GLFW_KEY_F17;
        case KeyCode::F18: return GLFW_KEY_F18;
        case KeyCode::F19: return GLFW_KEY_F19;
        case KeyCode::F20: return GLFW_KEY_F20;
        case KeyCode::F21: return GLFW_KEY_F21;
        case KeyCode::F22: return GLFW_KEY_F22;
        case KeyCode::F23: return GLFW_KEY_F23;
        case KeyCode::F24: return GLFW_KEY_F24;
        case KeyCode::F25: return GLFW_KEY_F25;

        case KeyCode::KP0: return GLFW_KEY_KP_0;
        case KeyCode::KP1: return GLFW_KEY_KP_1;
        case KeyCode::KP2: return GLFW_KEY_KP_2;
        case KeyCode::KP3: return GLFW_KEY_KP_3;
        case KeyCode::KP4: return GLFW_KEY_KP_4;
        case KeyCode::KP5: return GLFW_KEY_KP_5;
        case KeyCode::KP6: return GLFW_KEY_KP_6;
        case KeyCode::KP7: return GLFW_KEY_KP_7;
        case KeyCode::KP8: return GLFW_KEY_KP_8;
        case KeyCode::KP9: return GLFW_KEY_KP_9;
        case KeyCode::KPDecimal: return GLFW_KEY_KP_DECIMAL;
        case KeyCode::KPDivide: return GLFW_KEY_KP_DIVIDE;
        case KeyCode::KPMultiply: return GLFW_KEY_KP_MULTIPLY;
        case KeyCode::KPSubtract: return GLFW_KEY_KP_SUBTRACT;
        case KeyCode::KPAdd: return GLFW_KEY_KP_ADD;
        case KeyCode::KPEnter: return GLFW_KEY_KP_ENTER;
        case KeyCode::KPEqual: return GLFW_KEY_KP_EQUAL;

        case KeyCode::LeftShift: return GLFW_KEY_LEFT_SHIFT;
        case KeyCode::LeftControl: return GLFW_KEY_LEFT_CONTROL;
        case KeyCode::LeftAlt: return GLFW_KEY_LEFT_ALT;
        case KeyCode::LeftSuper: return GLFW_KEY_LEFT_SUPER;
        case KeyCode::RightShift: return GLFW_KEY_RIGHT_SHIFT;
        case KeyCode::RightControl: return GLFW_KEY_RIGHT_CONTROL;
        case KeyCode::RightAlt: return GLFW_KEY_RIGHT_ALT;
        case KeyCode::RightSuper: return GLFW_KEY_RIGHT_SUPER;
        case KeyCode::Menu: return GLFW_KEY_MENU;
        default: break;
        }
        return -1;
    }

    KeyCode GetKeyCodeFrom(int key) {

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

    KeyMods GetKeyModsFrom(int mods) {
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

    class GenericWindow: public Window, public MouseHandler, public KeyHandler {
    public:
        GenericWindow(DriverType driverType, InitConfiguration config, std::function<void()> initializer = [](){});
        virtual ~GenericWindow() override;
        virtual void Initialize() override;
        virtual void OnUpdate() override;
        virtual bool IsMouseButtonPressed(MouseCode button) override;
        virtual std::pair<float, float> GetMousePosition() override;
        virtual bool IsKeyPressed(KeyCode key) override;
        virtual void* GetNativeWindow() override;
    public:
        void InitializeEventHandler(GLFWwindow* m_window);
        
    #if defined(VWOLF_PLATFORM_MACOS) || defined(VWOLF_PLATFORM_IOS)
        inline NS::View* GetView() { return m_view; }
        inline void SetView(NS::View* view) { m_view = view; }
        inline NS::Window* GetCocoaWindow() { return reinterpret_cast<NS::Window*>(GetNativeWindow()); }
    #elif defined(VWOLF_PLATFORM_WINDOWS)
        inline HWND__* GetWin32Window() { return reinterpret_cast<HWND__*>(GetNativeWindow()); }
    #endif
    public:
        GLFWwindow* GetGLFWWindow() { return m_window; }
    private:
        void SetMousePosition(double xPos, double yPos);
        void SetMouseButtonState(int button, bool pressed);
        void SetKeyState(int key, bool pressed);
        void ClearInputState();
    private:
        std::function<void()> initializer;
        GLFWwindow *m_window;
        std::mutex m_inputMutex;
        float m_mouseX = 0.0f;
        float m_mouseY = 0.0f;
        bool m_mouseButtons[GLFW_MOUSE_BUTTON_LAST + 1] = {};
        bool m_keys[GLFW_KEY_LAST + 1] = {};
    #if defined(VWOLF_PLATFORM_MACOS) || defined(VWOLF_PLATFORM_IOS)
        NS::Window* m_nativeWindow;
        NS::View* m_view;
    #elif defined(VWOLF_PLATFORM_WINDOWS)
        HWND__* m_nativeWindow;
    #endif
    };

    Ref<Window> CreateGenericWindow(DriverType driverType, InitConfiguration config, std::function<void()> initializer) {
        return CreateRef<GenericWindow>(driverType, config, initializer);
    }

    GLFWwindow* GetGLFWWindow(Ref<Window> window) {
        return ((GenericWindow*)window.get())->GetGLFWWindow();
    }
#if defined(VWOLF_PLATFORM_MACOS) || defined(VWOLF_PLATFORM_IOS)
    NS::View* GetView(Ref<Window> window) {
        return ((GenericWindow*)window.get())->GetView();
    }

    void SetView(Ref<Window> window, NS::View* view) {
        return ((GenericWindow*)window.get())->SetView(view);
    }
#elif defined(VWOLF_PLATFORM_WINDOWS)

#endif

    GenericWindow::GenericWindow(DriverType driverType, InitConfiguration config, std::function<void()> initializer): Window(), initializer(initializer) {
        this->width = config.width;
        this->height = config.height;

        if (driverType != DriverType::OpenGL)
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        
        std::string appTitle = config.title;
        // TODO: For now, leave it to see the current driver type
        appTitle = appTitle + " - " + std::string(DriverName(driverType));
        m_window = glfwCreateWindow(width, height, appTitle.c_str(), NULL, NULL);
        if (m_window == NULL)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            Ref<WindowCloseEvent> evt = CreateRef<WindowCloseEvent>();
            EventQueue::DefaultQueue->Queue(evt);
            glfwTerminate();
        }
        const GLFWvidmode& mode = *glfwGetVideoMode(glfwGetPrimaryMonitor());
        int w = mode.width, h = mode.height;
        glfwSetWindowPos(m_window, (w / 2) - (width / 2), (h / 2) - (height / 2));
        if (config.maximize)
            glfwMaximizeWindow(m_window);
        if (driverType == DriverType::OpenGL)
            glfwMakeContextCurrent(m_window);
        
#if defined(VWOLF_PLATFORM_MACOS) || defined(VWOLF_PLATFORM_IOS)
        void * window = glfwGetCocoaWindow(m_window);
        m_nativeWindow = reinterpret_cast<NS::Window*>(glfwGetCocoaWindow(m_window));
        VWOLF_CORE_ASSERT(window == m_nativeWindow);
#elif defined(VWOLF_PLATFORM_WINDOWS)
        void* window = glfwGetWin32Window(m_window);
        m_nativeWindow = reinterpret_cast<HWND__*>(glfwGetWin32Window(m_window));
		VWOLF_CORE_ASSERT(window == m_nativeWindow);
#endif
        
        InitializeEventHandler(m_window);
    }

    GenericWindow::~GenericWindow() {
        glfwDestroyWindow(m_window);
    }

    void GenericWindow::InitializeEventHandler(GLFWwindow* m_window) {
        // Setting events
        glfwSetWindowUserPointer(m_window, this);
        glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window)
        {
            Ref<WindowCloseEvent> evt = CreateRef<WindowCloseEvent>();
            EventQueue::DefaultQueue->Queue(evt);
        });

        glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int width, int height)
        {
            GenericWindow& data = *(GenericWindow*)glfwGetWindowUserPointer(window);
            data.SetWidth(width);
            data.SetHeight(height);
            
            int framebufferWidth;
            int framebufferHeight;
            glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
            data.SetFramebufferWidth(framebufferWidth);
            data.SetFramebufferHeight(framebufferHeight);

            Ref<WindowResizeEvent> evt = CreateRef<WindowResizeEvent>(width, height);
            EventQueue::DefaultQueue->Queue(evt);
        });

        glfwSetWindowFocusCallback(m_window, [](GLFWwindow* window, int focused)
        {
            if (!focused) {
                GenericWindow& data = *(GenericWindow*)glfwGetWindowUserPointer(window);
                data.ClearInputState();
            }
        });

        glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double xPos, double yPos)
        {
            GenericWindow& data = *(GenericWindow*)glfwGetWindowUserPointer(window);
            data.SetMousePosition(xPos, yPos);

            Ref<MouseMovedEvent> evt = CreateRef<MouseMovedEvent>(xPos, yPos);
            EventQueue::DefaultQueue->Queue(evt);
        });

        glfwSetScrollCallback(m_window, [](GLFWwindow* window, double xOffset, double yOffset)
        {
            Ref<MouseScrolledEvent> evt = CreateRef<MouseScrolledEvent>(xOffset, yOffset);
            EventQueue::DefaultQueue->Queue(evt);
        });

        glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods)
        {
            GenericWindow& data = *(GenericWindow*)glfwGetWindowUserPointer(window);
            data.SetMouseButtonState(button, action != GLFW_RELEASE);

            switch (action) {
            case GLFW_PRESS:
            {
                Ref<MouseButtonPressedEvent> evt = CreateRef<MouseButtonPressedEvent>(GetMouseCode(button));
                EventQueue::DefaultQueue->Queue(evt);
            }
            break;
            case GLFW_RELEASE:
            {
                Ref<MouseButtonReleasedEvent> evt = CreateRef<MouseButtonReleasedEvent>(GetMouseCode(button));
                EventQueue::DefaultQueue->Queue(evt);
            }
            break;
            }
        });

        glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
        {
            GenericWindow& data = *(GenericWindow*)glfwGetWindowUserPointer(window);
            if (action != GLFW_REPEAT) {
                data.SetKeyState(key, action == GLFW_PRESS);
            }

            switch (action)
            {
            case GLFW_PRESS:
            {
                Ref<KeyPressedEvent> evt = CreateRef<KeyPressedEvent>(GetKeyCodeFrom(key), GetKeyModsFrom(mods), 0);
                EventQueue::DefaultQueue->Queue(evt);
                break;
            }
            case GLFW_RELEASE:
            {
                Ref<KeyReleasedEvent> evt = CreateRef<KeyReleasedEvent>(GetKeyCodeFrom(key), GetKeyModsFrom(mods));
                EventQueue::DefaultQueue->Queue(evt);
                break;
            }
            case GLFW_REPEAT:
            {
                Ref<KeyPressedEvent> evt = CreateRef<KeyPressedEvent>(GetKeyCodeFrom(key), GetKeyModsFrom(mods), 0);
                EventQueue::DefaultQueue->Queue(evt);
                break;
            }
            }
        });

        glfwSetCharCallback(m_window, [](GLFWwindow* window, unsigned int keycode)
        {
            Ref<KeyTypedEvent> evt = CreateRef<KeyTypedEvent>(boost::lexical_cast<std::string>(static_cast<unsigned char>(keycode)));
            EventQueue::DefaultQueue->Queue(evt);
        });
        
        glfwSetDropCallback(m_window,  [](GLFWwindow* window, int path_count, const char* paths[]) {
            Ref<WindowDragDropEvent> evt = CreateRef<WindowDragDropEvent>(path_count, paths);
            EventQueue::DefaultQueue->Queue(evt);
        });
    }

    void GenericWindow::Initialize() {
        initializer();
    }

    void GenericWindow::OnUpdate() {
        glfwPollEvents();
    }

    void GenericWindow::SetMousePosition(double xPos, double yPos) {
        // Not sure if clamping is the right way to go
        xPos = std::clamp((int)xPos, 0, width);
        yPos = std::clamp((int)yPos, 0, height);

        std::scoped_lock lock(m_inputMutex);
        m_mouseX = static_cast<float>(xPos);
        m_mouseY = static_cast<float>(yPos);
    }

    void GenericWindow::SetMouseButtonState(int button, bool pressed) {
        if (button < 0 || button > GLFW_MOUSE_BUTTON_LAST) {
            return;
        }

        std::scoped_lock lock(m_inputMutex);
        m_mouseButtons[button] = pressed;
    }

    void GenericWindow::SetKeyState(int key, bool pressed) {
        if (key < 0 || key > GLFW_KEY_LAST) {
            return;
        }

        std::scoped_lock lock(m_inputMutex);
        m_keys[key] = pressed;
    }

    void GenericWindow::ClearInputState() {
        std::scoped_lock lock(m_inputMutex);
        m_mouseX = 0.0f;
        m_mouseY = 0.0f;
        for (int button = 0; button <= GLFW_MOUSE_BUTTON_LAST; ++button) {
            m_mouseButtons[button] = false;
        }
        for (int key = 0; key <= GLFW_KEY_LAST; ++key) {
            m_keys[key] = false;
        }
    }

    bool GenericWindow::IsMouseButtonPressed(MouseCode button) {
        const int glfwButton = GetMouseFrom(button);
        if (glfwButton < 0 || glfwButton > GLFW_MOUSE_BUTTON_LAST) {
            return false;
        }

        std::scoped_lock lock(m_inputMutex);
        return m_mouseButtons[glfwButton];
    }

    std::pair<float, float> GenericWindow::GetMousePosition() {
        std::scoped_lock lock(m_inputMutex);
        return { m_mouseX, m_mouseY };
    }

    bool GenericWindow::IsKeyPressed(KeyCode key) {
        const int glfwKey = GetKeyFrom(key);
        if (glfwKey < 0 || glfwKey > GLFW_KEY_LAST) {
            return false;
        }

        std::scoped_lock lock(m_inputMutex);
        return m_keys[glfwKey];
    }

    void* GenericWindow::GetNativeWindow() {
#if defined(VWOLF_PLATFORM_MACOS) || defined(VWOLF_PLATFORM_IOS)
        return m_nativeWindow;
#elif defined(VWOLF_PLATFORM_WINDOWS)
        return m_nativeWindow;
#endif
    }
}
