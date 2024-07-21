//
//  Toolbar.cpp
//  VWolfPup
//
//  Created by Enrique Ricalde on 6/23/24.
//

#include "Toolbar.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace VWolfPup {
    Toolbar::Toolbar(std::function<void(bool)> onPlayPressed): View("Toolbar"), onPlayPressed(onPlayPressed) {
        window_class = new ImGuiWindowClass;
        window_class->DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar;
    }

    Toolbar::~Toolbar() {
        delete window_class;
    }

    void Toolbar::SetInContainer() {
        GetContainer()->GetRoot()->Install(this, ImGuiDir_Up);
    }

    void Toolbar::OnGui() {
        ImGui::SetNextWindowClass(window_class);
        ImGui::Begin(title.c_str());

        bool shouldChangeColor = false;
        if (isPlaying) {
            shouldChangeColor = true;
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
        }
        auto windowWidth = ImGui::GetWindowSize().x;
        auto textWidth   = ImGui::CalcTextSize(">").x;
        ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
        wasPressed = ImGui::Button(">");
        if (wasPressed) {
            isPlaying = !isPlaying;
            onPlayPressed(isPlaying);
            wasPressed = false;
        }
        if (shouldChangeColor) {
            ImGui::PopStyleColor(3);
        }
        ImGui::End();
    }
}
