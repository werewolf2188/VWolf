//
//  SceneHierarchy.cpp
//  VWolfPup
//
//  Created by Enrique Ricalde on 4/13/23.
//

#include "SceneHierarchy.h"

#include <imgui/imgui.h>

namespace VWolfPup {
    void DrawGameObject(VWolf::Ref<VWolf::GameObject> object, std::function<void(VWolf::Ref<VWolf::GameObject>)> onTapped,
                        std::string& selectedName, bool& didSelection) {
        bool isItemClicked = false;
        if (ImGui::TreeNodeEx(object->GetName().c_str(),
                              ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_Leaf
                              | ( selectedName == object->GetName() ? ImGuiTreeNodeFlags_Selected : ImGuiTreeNodeFlags_None ))) {

            isItemClicked = ImGui::IsItemClicked();
            ImGui::TreePop();
        }
        if (isItemClicked) {
            selectedName = object->GetName();
            onTapped(object);
            didSelection = true;
        }
    }

    SceneHierarchy::SceneHierarchy(VWolf::Scene *scene, std::function<void(VWolf::Ref<VWolf::GameObject>)> onTapped):
    View("Scene Hierarchy"), scene(scene), onTapped(onTapped) { }

    SceneHierarchy::~SceneHierarchy() {

    }

    void SceneHierarchy::SetInContainer() {
        GetContainer()->GetRoot()->Install(this, ImGuiDir_Left);
    }

    void SceneHierarchy::OnGui() {
        ImGui::Begin(title.c_str());

        if (ImGui::TreeNodeEx(scene->GetName().c_str(),
                              ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_OpenOnArrow)) {
            for(VWolf::Ref<VWolf::GameObject> gameObject: scene->GetGameObjects()){
                DrawGameObject(gameObject, onTapped, selectedName, didSelection);
            }
            ImGui::TreePop();
        }
        if (showDialog) {
            if (ImGui::BeginPopupContextWindow("Options"))
            {
                didSelection = true;
                if (ImGui::MenuItem("Add New Game Object"))
                {
                    showDialog = false;
                    auto gameObject = scene->CreateGameObject("Untitled");
                    selectedName = gameObject->GetName();
                    onTapped(gameObject);
                }
                else if (ImGui::MenuItem("Add New Shape"))
                {
                    showDialog = false;
                    auto gameObject = scene->CreateGameObject("Untitled");
                    gameObject->AddComponent<VWolf::ShapeRendererComponent>();
                    selectedName = gameObject->GetName();
                    onTapped(gameObject);
                }
                else if (ImGui::MenuItem("Add Light"))
                {
                    showDialog = false;
                    auto gameObject = scene->CreateGameObject("Untitled");
                    gameObject->AddComponent<VWolf::LightComponent>();
                    selectedName = gameObject->GetName();
                    onTapped(gameObject);
                }
                else if (!selectedName.empty() && ImGui::MenuItem("Delete Selected")) {
                    // Here I can delete the selected one
                    onTapped(nullptr);
                    scene->RemoveGameObject(selectedName);
                }

                ImGui::EndPopup();
            }
        }
        ImGui::End();
    }

    void SceneHierarchy::OnEvent(VWolf::Event& evt) {
        VWolf::Dispatch<VWolf::MouseButtonReleasedEvent>(evt, VWOLF_BIND_EVENT_FN(SceneHierarchy::OnMouseButtonReleasedEvent));
    }

    bool SceneHierarchy::OnMouseButtonReleasedEvent(VWolf::MouseButtonReleasedEvent& e) {
        if (e.GetMouseButton() == VWolf::MouseCode::Right) {
            showDialog = true;
        }
        else if (e.GetMouseButton() == VWolf::MouseCode::Left && !didSelection) {
            selectedName = "";
        }
        didSelection = false;
        return true;
    }
}
