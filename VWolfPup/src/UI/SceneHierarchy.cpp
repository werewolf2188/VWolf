//
//  SceneHierarchy.cpp
//  VWolfPup
//
//  Created by Enrique Ricalde on 4/13/23.
//

#include "SceneHierarchy.h"

#include <imgui/imgui.h>

namespace VWolfPup {
    void DrawGameObject(VWolf::Ref<VWolf::GameObject> object, std::function<void(VWolf::Ref<VWolf::GameObject>)> onTapped) {
        if (ImGui::Selectable(object->GetName().c_str())) {
            onTapped(object);
        }
    }

    SceneHierarchy::SceneHierarchy(VWolf::Scene *scene, std::function<void(VWolf::Ref<VWolf::GameObject>)> onTapped):
    View("Scene Hierarchy"), scene(scene), onTapped(onTapped) { }

    SceneHierarchy::~SceneHierarchy() {

    }

    void SceneHierarchy::OnGui() {
        ImGui::Begin(title.c_str());
        if (ImGui::TreeNode(scene->GetName().c_str())) {
            for(VWolf::Ref<VWolf::GameObject> gameObject: scene->GetGameObjects()){
                DrawGameObject(gameObject, onTapped);
            }
            ImGui::TreePop();
        }
        ImGui::End();
    }
}
