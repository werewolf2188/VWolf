//
//  Toolbar.h
//  VWolfPup
//
//  Created by Enrique Ricalde on 6/23/24.
//

#pragma once

#include "UICore.h"
#include "VWolf.h"

#include <functional>

struct ImGuiWindowClass;

namespace VWolfPup {
    class Toolbar: public View {
    public:
        Toolbar(std::function<void(bool)> onPlayPressed);
        ~Toolbar();
    public:
        void OnGui() override;
    protected:
        virtual void SetInContainer() override;
    private:
        ImGuiWindowClass* window_class;
        bool wasPressed = false;
        bool isPlaying = false;
        std::function<void(bool)> onPlayPressed;
    };
}

