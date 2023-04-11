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
    void SceneSettings::OnGui() {
        ImGui::Begin(title.c_str());
        ImGui::LabelText("Background", "");
        ImGui::ColorEdit4("Background Color", VWolf::value_ptr(scene->GetSceneBackground().GetBackgroundColor()));
        ImGui::End();
    }
}
