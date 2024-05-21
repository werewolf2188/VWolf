//
//  Common.hpp
//  VWolf
//
//  Created by Enrique Ricalde on 4/21/24.
//

#pragma once

#include "VWolf/Core/Window.h"

struct GLFWwindow;

namespace VWolf {

    enum class KeyCode;
    enum class KeyMods;
    enum class MouseCode;

    KeyCode GetKeyCodeFrom(int key);
    int GetKeyFrom(KeyCode key);
    KeyMods GetKeyModsFrom(int mods);
    int GetMouseFrom(MouseCode button);
    MouseCode GetMouseCode(int button);

    class IWindowCallback {
    public:
        void InitializeEventHandler(GLFWwindow* m_window);
    public:
        virtual WindowEventCallback& GetCallback() = 0;
        virtual void SetWidth(int width) = 0;
        virtual void SetHeight(int height) = 0;
    };
}
