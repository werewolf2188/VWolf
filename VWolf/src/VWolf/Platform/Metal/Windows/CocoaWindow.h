//
//  CocoaWindow.hpp
//  VWolf
//
//  Created by Enrique Ricalde on 4/19/24.
//

#pragma once

#include "VWolf/Core/Window.h"
#include "VWolf/Core/Events/MouseEvent.h"
#include "VWolf/Core/Events/KeyEvent.h"

#include "VWolf/Platform/GLFWCommon.h"

#include "VWolf/Platform/Metal/Core/Core.h"

struct GLFWwindow;

namespace VWolf {
    class CocoaWindow : public Window, public MouseHandler, public KeyHandler, public IWindowCallback {
    public: // Inherits
        CocoaWindow(InitConfiguration config, WindowEventCallback& callback);
        virtual ~CocoaWindow() override;
        virtual void Initialize() override;
        virtual void OnUpdate() override;
        virtual bool IsMouseButtonPressed(MouseCode button) override;
        virtual std::pair<float, float> GetMousePosition() override;
        virtual bool IsKeyPressed(KeyCode key) override;
        virtual void* GetNativeWindow() override { return m_nativeWindow; };
    public:
        virtual WindowEventCallback& GetCallback() override { return callback; }
        virtual void SetWidth(int width) override { this->width = width; }
        virtual void SetHeight(int height) override { this->height = height; }
    public:
        inline NS::View* GetView() { return m_view; }
        inline void SetView(NS::View* view) { m_view = view; }
    private:
        GLFWwindow* m_window;
        NS::Window* m_nativeWindow;
        NS::View* m_view;
        WindowEventCallback& callback;
    };
}

