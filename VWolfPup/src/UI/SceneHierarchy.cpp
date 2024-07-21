//
//  SceneHierarchy.cpp
//  VWolfPup
//
//  Created by Enrique Ricalde on 4/13/23.
//

#include "SceneHierarchy.h"

#include <imgui/imgui.h>
#include <filesystem>

#define SELECT_PRIMITIVE(T) \
if (ImGui::MenuItem(T)) {\
    auto path = GetPrimitivePath(T); \
    showDialog = false; \
    auto gameObject = scene->CreateGameObject("Untitled"); \
    gameObject->AddComponent<VWolf::MeshFilterComponent>(); \
    gameObject->GetComponent<VWolf::MeshFilterComponent>().SetPath(path); \
    gameObject->AddComponent<VWolf::MeshRendererComponent>(); \
    selectedName = gameObject->GetName(); \
    onTapped(gameObject); \
} \

namespace VWolfPup {

    std::filesystem::path GetPrimitivePath(const char* name) {
        if (strcmp(name, "Box") == 0) {
            return std::filesystem::current_path() / "assets/basic_shapes/Box.obj";
        }
        else if (strcmp(name, "Sphere") == 0) {
            return std::filesystem::current_path() / "assets/basic_shapes/Sphere.obj";
        }
        else if (strcmp(name, "Geosphere") == 0) {
            return std::filesystem::current_path() / "assets/basic_shapes/Geosphere.obj";
        }
        else if (strcmp(name, "Cylinder") == 0) {
            return std::filesystem::current_path() / "assets/basic_shapes/Cylinder.obj";
        }
        else if (strcmp(name, "Grid") == 0) {
            return std::filesystem::current_path() / "assets/basic_shapes/Grid.obj";
        }
        else if (strcmp(name, "Monkey") == 0) {
            return std::filesystem::current_path() / "assets/basic_shapes/Monkey.obj";
        }
        return std::filesystem::path();
    }

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
//                else if (ImGui::MenuItem("Add New Shape"))
//                {
//                    showDialog = false;
//                    auto gameObject = scene->CreateGameObject("Untitled");
//                    gameObject->AddComponent<VWolf::ShapeRendererComponent>();
//                    selectedName = gameObject->GetName();
//                    onTapped(gameObject);
//                }
                if (ImGui::BeginMenu("Add Object")) {

                    SELECT_PRIMITIVE("Box")
                    SELECT_PRIMITIVE("Sphere")
                    SELECT_PRIMITIVE("Geosphere")
                    SELECT_PRIMITIVE("Cylinder")
                    SELECT_PRIMITIVE("Grid")
                    SELECT_PRIMITIVE("Monkey")
                    ImGui::EndMenu();
                }
                
                if (ImGui::MenuItem("Add Light"))
                {
                    showDialog = false;
                    auto gameObject = scene->CreateGameObject("Untitled");
                    gameObject->AddComponent<VWolf::LightComponent>();
                    selectedName = gameObject->GetName();
                    onTapped(gameObject);
                }

                if (ImGui::MenuItem("Add Camera"))
                {
                    showDialog = false;
                    auto gameObject = scene->CreateGameObject("Untitled");
                    gameObject->AddComponent<VWolf::CameraComponent>();
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
