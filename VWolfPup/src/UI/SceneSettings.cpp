//
//  SceneSettings.cpp
//  VWolfPup
//
//  Created by Enrique Ricalde on 4/17/23.
//

#include "SceneSettings.h"
#include <imgui/imgui.h>

namespace VWolfPup {
    SceneSettings::SceneSettings(VWolf::Scene *scene): scene(scene), View("Scene Settings") {
        
    }

    SceneSettings::~SceneSettings() {
        
    }

    void SceneSettings::SetInContainer() {
        GetContainer()->GetRoot()->Install(this, ImGuiDir_Right);
    }

    void SceneSettings::OnGui() {
        ImGui::Begin(title.c_str());
        ImGui::PushID("Background");

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, 100.0f);
        ImGui::Text("%s", "Background");
        ImGui::NextColumn();

        ImGui::PushItemWidth(ImGui::CalcItemWidth());
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

        ImGui::ColorEdit4("##Background Color", VWolf::value_ptr(scene->GetSceneBackground().GetBackgroundColor()), ImGuiColorEditFlags_NoInputs);
        
        ImGui::PopStyleVar();
        ImGui::PopItemWidth();
        ImGui::Columns(1);

        ImGui::PopID();
        ImGui::End();
    }
}
