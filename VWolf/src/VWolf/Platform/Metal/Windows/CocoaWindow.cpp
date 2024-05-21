//
//  CocoaWindow.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 4/19/24.
//

#include "vwpch.h"

#if defined(VWOLF_PLATFORM_MACOS) || defined(VWOLF_PLATFORM_IOS)
#include "CocoaWindow.h"

#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>

#include "VWolf/Core/Events/ApplicationEvent.h"

namespace VWolf {
    CocoaWindow::CocoaWindow(InitConfiguration config, WindowEventCallback& callback): Window(), callback(callback) {
        this->width = config.width;
        this->height = config.height;

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        m_window = glfwCreateWindow(width, height, config.title, NULL, NULL);
        if (m_window == NULL)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            WindowCloseEvent evt;
            GetCallback().OnEvent(evt);
            glfwTerminate();
        }
        const GLFWvidmode& mode = *glfwGetVideoMode(glfwGetPrimaryMonitor());
        int w = mode.width, h = mode.height;
        glfwSetWindowPos(m_window, (w / 2) - (width / 2), (h / 2) - (height / 2));
        if (config.maximize)
            glfwMaximizeWindow(m_window);

        void * window = glfwGetCocoaWindow(m_window);
        m_nativeWindow = reinterpret_cast<NS::Window*>(glfwGetCocoaWindow(m_window));
        VWOLF_CORE_ASSERT(window == m_nativeWindow);

        InitializeEventHandler(m_window);
    }

    CocoaWindow::~CocoaWindow() {
        glfwDestroyWindow(m_window);
    }

    void CocoaWindow::Initialize() {

    }

    void CocoaWindow::OnUpdate() {
        glfwPollEvents();
    }

    bool CocoaWindow::IsMouseButtonPressed(MouseCode button) {
        return glfwGetMouseButton(m_window, GetMouseFrom(button)) == GLFW_PRESS;
    }

    std::pair<float, float> CocoaWindow::GetMousePosition() {
        double xpos, ypos;
        glfwGetCursorPos(m_window, &xpos, &ypos);
        // Not sure if clamping is the right way to go
        xpos = std::clamp((int)xpos, 0, width);
        ypos = std::clamp((int)ypos, 0, height);
        return std::make_pair<float, float>((float)xpos, (float)ypos);
    }

    bool CocoaWindow::IsKeyPressed(KeyCode key) {
        return glfwGetKey(m_window, GetKeyFrom(key)) == GLFW_PRESS;
    }
}

#endif
