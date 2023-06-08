//
//  SceneSettings.cpp
//  VWolfPup
//
//  Created by Enrique Ricalde on 4/17/23.
//

#include "SceneSettings.h"
#include <imgui/imgui.h>

namespace VWolfPup {
    SceneSettings::SceneSettings(VWolf::Scene *scene):
    scene(scene), selection((int)scene->GetSceneBackground().GetType()), View("Scene Settings") {
        
    }

    SceneSettings::~SceneSettings() {
        
    }

    void SceneSettings::SetInContainer() {
        GetContainer()->GetRoot()->Install(this, ImGuiDir_Right);
    }

    void SceneSettings::OnGui() {
        ImGui::Begin(title.c_str());
        ImGui::PushID("Background_Type");

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, 100.0f);
        ImGui::Text("%s", "Background Type");
        ImGui::NextColumn();

        ImGui::PushItemWidth(ImGui::CalcItemWidth());
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

        if (ImGui::Combo("##Background_Type", &selection, items.data(), (int)items.size())) {
            scene->GetSceneBackground().SetType((VWolf::SceneBackground::Type)selection);
        }
        
        ImGui::PopStyleVar();
        ImGui::PopItemWidth();
        ImGui::Columns(1);

        ImGui::PopID();
        if (selection == (int)VWolf::SceneBackground::Type::Color) {
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
        }
        ImGui::End();
    }
}
