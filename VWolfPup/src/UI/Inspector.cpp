//
//  Inspector.cpp
//  VWolfPup
//
//  Created by Enrique Ricalde on 4/14/23.
//

#include "Inspector.h"

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <type_traits>

#include "ObjectExplorer.h"

#include "../LoadSettings.h"

#include "../ProjectManagement/Project.h"
#include "../ProjectManagement/Extensions.h"

template<typename T, typename UIFunction>
static bool DrawComponent(const std::string& name, T& component, UIFunction uiFunction)
{
    const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;

    ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
    float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
    ImGui::Separator();
    bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, "%s", name.c_str());
    ImGui::PopStyleVar(
    );
    
    if (!std::is_same<T, VWolf::TransformComponent>::value) {
        ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
        if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight }))
        {
            ImGui::OpenPopup("ComponentSettings");
        }
    }

    bool removeComponent = false;
    if (ImGui::BeginPopup("ComponentSettings"))
    {
        if (ImGui::MenuItem("Remove component"))
            removeComponent = true;

        ImGui::EndPopup();
    }

    if (open)
    {
        uiFunction(component);
        ImGui::TreePop();
    }

    return removeComponent;

}

static void DrawVec3Control(const std::string& label, VWolf::Vector3& values, float resetValue = 0.0f, float columnWidth = 100.0f)
{
    ImGuiIO& io = ImGui::GetIO();
    auto boldFont = io.Fonts->Fonts[0];

    ImGui::PushID(label.c_str());

    ImGui::Columns(2);
    ImGui::SetColumnWidth(0, columnWidth);
    ImGui::Text("%s", label.c_str());
    ImGui::NextColumn();

    ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

    float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
    ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
    ImGui::PushFont(boldFont);
    if (ImGui::Button("X", buttonSize))
        values.SetX(resetValue);
    ImGui::PopFont();
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    ImGui::DragFloat("##X", &values.GetX(), 0.1f, 0.0f, 0.0f, "%.2f");
    ImGui::PopItemWidth();
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
    ImGui::PushFont(boldFont);
    if (ImGui::Button("Y", buttonSize))
        values.SetY(resetValue);
    ImGui::PopFont();
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    ImGui::DragFloat("##Y", &values.GetY(), 0.1f, 0.0f, 0.0f, "%.2f");
    ImGui::PopItemWidth();
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
    ImGui::PushFont(boldFont);
    if (ImGui::Button("Z", buttonSize))
        values.SetZ(resetValue);
    ImGui::PopFont();
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    ImGui::DragFloat("##Z", &values.GetZ(), 0.1f, 0.0f, 0.0f, "%.2f");
    ImGui::PopItemWidth();

    ImGui::PopStyleVar();

    ImGui::Columns(1);

    ImGui::PopID();
}

static bool ButtonCenteredOnLine(const char* label, float alignment = 0.5f)
{
    ImGuiStyle& style = ImGui::GetStyle();

    float size = ImGui::CalcTextSize(label).x + style.FramePadding.x * 2.0f;
    float avail = ImGui::GetContentRegionAvail().x;

    float off = (avail - size) * alignment;
    if (off > 0.0f)
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);

    return ImGui::Button(label);
}

namespace VWolfPup {

    class TransformComponentInspector: public VWolf::ComponentInspector<VWolf::TransformComponent> {
    public:
        TransformComponentInspector() {}
        ~TransformComponentInspector() {}
    public:
        virtual void OnInspector(VWolf::TransformComponent* component) override {
            DrawComponent<VWolf::TransformComponent>(component->GetName(), *component, [](VWolf::TransformComponent& component) {
                DrawVec3Control("Position", component.GetPosition());
                DrawVec3Control("Rotation", component.GetEulerAngles());
                DrawVec3Control("Scale", component.GetLocalScale(), 1.0);
            });
        }
    };

    class LightComponentInspector: public VWolf::ComponentInspector<VWolf::LightComponent> {
    public:
        LightComponentInspector() {}
        ~LightComponentInspector() {}
    public:
        virtual void OnInspector(VWolf::LightComponent* component) override {
            auto remove = DrawComponent<VWolf::LightComponent>(component->GetName(), *component, [this](VWolf::LightComponent& component) {
                
                VWolf::Light::LightType currentType = component.GetLight().type;
                auto lambda = [this, currentType](const char* name) {
                    return strcmp(Name(currentType), name) == 0;
                };
                auto value = std::find_if(items.begin(), items.end(), lambda);
                selection = (int)std::distance(items.begin(), value);

                ImGui::PushID("Type");

                ImGui::Columns(2);
                ImGui::SetColumnWidth(0, 100.0f);
                ImGui::Text("%s", "Type");
                ImGui::NextColumn();

                ImGui::PushItemWidth(ImGui::CalcItemWidth());
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
                if (ImGui::Combo("##TypeSelector", &selection, items.data(), (int)items.size())) {
                    component.GetLight().type = Type(items[selection]);
                }
                ImGui::PopStyleVar();
                ImGui::PopItemWidth();
                ImGui::Columns(1);

                ImGui::PopID();
                
                ImGui::PushID("Color");

                ImGui::Columns(2);
                ImGui::SetColumnWidth(0, 100.0f);
                ImGui::Text("%s", "Color");
                ImGui::NextColumn();

                ImGui::PushItemWidth(ImGui::CalcItemWidth());
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
                
                ImGui::ColorEdit4("##Light Color", &component.GetLight().color.GetX(), ImGuiColorEditFlags_NoInputs);

                ImGui::PopStyleVar();
                ImGui::PopItemWidth();
                ImGui::Columns(1);

                ImGui::PopID();
                
                ImGui::PushID("Strength");

                ImGui::Columns(2);
                ImGui::SetColumnWidth(0, 100.0f);
                ImGui::Text("%s", "Strength");
                ImGui::NextColumn();

                ImGui::PushItemWidth(ImGui::CalcItemWidth());
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

                ImGui::DragFloat3("##Light Strength", &component.GetLight().strength.GetX(), 0.1f, 0, 1);

                ImGui::PopStyleVar();
                ImGui::PopItemWidth();
                ImGui::Columns(1);

                ImGui::PopID();
                if (component.GetLight().type == VWolf::Light::LightType::Spot) {
                    ImGui::PushID("CutOff");

                    ImGui::Columns(2);
                    ImGui::SetColumnWidth(0, 100.0f);
                    ImGui::Text("%s", "CutOff");
                    ImGui::NextColumn();

                    ImGui::PushItemWidth(ImGui::CalcItemWidth());
                    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
                    ImGui::DragFloat("##Light CutOff", &component.GetLight().cutOff, 0.1f, 0, VWolf::radians(90.0f));
                    
                    ImGui::PopStyleVar();
                    ImGui::PopItemWidth();
                    ImGui::Columns(1);

                    ImGui::PopID();
                    
                    ImGui::PushID("Exponent");

                    ImGui::Columns(2);
                    ImGui::SetColumnWidth(0, 100.0f);
                    ImGui::Text("%s", "Exponent");
                    ImGui::NextColumn();

                    ImGui::PushItemWidth(ImGui::CalcItemWidth());
                    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
                    ImGui::DragFloat("##Light Exponent", &component.GetLight().exponent, 0.1f, 0, 100);
                    
                    ImGui::PopStyleVar();
                    ImGui::PopItemWidth();
                    ImGui::Columns(1);

                    ImGui::PopID();
                }
            });
            if (remove)
                component->GetGameObject()->RemoveComponent<VWolf::LightComponent>();
        }
    private:
        const char * Name(VWolf::Light::LightType type) {
            switch (type) {
                case VWolf::Light::LightType::Directional: return items[0];
                case VWolf::Light::LightType::Point: return items[1];
                case VWolf::Light::LightType::Spot: return items[2];
                default:
                    return items[0];
            }
        }
        
        VWolf::Light::LightType Type(const char* name) {
            if (strcmp(name, items[0]) == 0)
                return VWolf::Light::LightType::Directional;
            else if (strcmp(name, items[1]) == 0)
                return VWolf::Light::LightType::Point;
            else if (strcmp(name, items[2]) == 0)
                return VWolf::Light::LightType::Spot;
            return VWolf::Light::LightType::Unknown;
        }
    private:
        int selection = 0;
        std::array<const char*, 3> items = { "Directional", "Point", "Spot" };
    };

    class MeshFilterComponentInspector: public VWolf::ComponentInspector<VWolf::MeshFilterComponent> {
    public:
        MeshFilterComponentInspector() {}
        ~MeshFilterComponentInspector() {}
    public:
        virtual void OnInspector(VWolf::MeshFilterComponent* component) override {
            auto remove = DrawComponent<VWolf::MeshFilterComponent>(component->GetName(), *component, [this](VWolf::MeshFilterComponent& component) {
                ImGui::PushID("Mesh Filter");

                ImGui::Columns(3);
                ImGui::SetColumnWidth(0, 100.0f);
                ImGui::Text("%s", "Mesh");
                ImGui::NextColumn();

                ImGui::PushItemWidth(ImGui::CalcItemWidth());
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
                ImGui::Text("%s", component.GetPath().filename().string().c_str());
                ImGui::PopStyleVar();
                
                ImGui::PopItemWidth();
                ImGui::NextColumn();
                float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
                VWolf::MeshFilterComponent* comp = &component;
                if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight }))
                {
                    ContainerView::GetMainView()
                    ->AddView(new ObjectExplorer(".obj", [comp](auto path){
//                        VWOLF_CLIENT_INFO("Test");
                        comp->SetPath(path);
                    }));
                }
                ImGui::Columns(1);

                ImGui::PopID();
            });
            if (remove) {
                component->GetGameObject()->RemoveComponent<VWolf::MeshRendererComponent>();
                component->GetGameObject()->RemoveComponent<VWolf::MeshFilterComponent>();
            }
        }
    
    private:
    };

    class MeshRendererComponentInspector: public VWolf::ComponentInspector<VWolf::MeshRendererComponent> {
    public:
        MeshRendererComponentInspector() {}
        ~MeshRendererComponentInspector() {}
    public:
        virtual void OnInspector(VWolf::MeshRendererComponent* component) override {
            auto remove = DrawComponent<VWolf::MeshRendererComponent>(component->GetName(), *component, [this](VWolf::MeshRendererComponent& component) {
                ImGui::PushID("Mesh Filter");

                ImGui::Columns(3);
                ImGui::SetColumnWidth(0, 100.0f);
                ImGui::Text("%s", "Material");
                ImGui::NextColumn();

                ImGui::PushItemWidth(ImGui::CalcItemWidth());
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
                ImGui::Text("%s", component.GetMaterial().GetName().c_str());
                ImGui::PopStyleVar();
                ImGui::NextColumn();
                float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
                if (ImGui::Button("...", ImVec2{ lineHeight, lineHeight }))
                {
                    ContainerView::GetMainView()
                    ->AddView(new ObjectExplorer(Extension::GetMaterialExtension(), [this, &component](auto path){
        //                        VWOLF_CLIENT_INFO("Test");
        //                comp->SetPath(path);
                        auto material = Project::CurrentProject()->GetMaterial(path);
                        if (material) {
                            component.SetMaterial(material.get());
                        } else {
                            component.SetMaterial(Defaults::Get()->GetDefaultMaterial().get());                            
                        }
                    }));
                }
                
                ImGui::Columns(1);

                ImGui::PopID();
            });
            if (remove) {
                component->GetGameObject()->RemoveComponent<VWolf::MeshRendererComponent>();
                component->GetGameObject()->RemoveComponent<VWolf::MeshFilterComponent>();
            }
        }

    private:
    };

    class ShapeRendererComponentInspector: public VWolf::ComponentInspector<VWolf::ShapeRendererComponent> {
    public:
        ShapeRendererComponentInspector() {}
        ~ShapeRendererComponentInspector() {}
    public:
        virtual void OnInspector(VWolf::ShapeRendererComponent* component) override {
            auto remove = DrawComponent<VWolf::ShapeRendererComponent>(component->GetName(), *component, [this](VWolf::ShapeRendererComponent& component) {
                std::string currentName = component.GetData().GetName();

                auto lambda = [currentName](const char* name) {
                    return strcmp(currentName.c_str(), name) == 0;
                };
                
                auto value = std::find_if(items.begin(), items.end(), lambda);
                selection = (int)std::distance(items.begin(), value);

                ImGui::PushID("Shape");

                ImGui::Columns(2);
                ImGui::SetColumnWidth(0, 100.0f);
                ImGui::Text("%s", "Shape");
                ImGui::NextColumn();

                ImGui::PushItemWidth(ImGui::CalcItemWidth());
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
                if (ImGui::Combo("##ShapeSelector", &selection, items.data(), (int)items.size())) {
                    
                    component.SetData(GetMeshData());
                }
                ImGui::PopStyleVar();
                ImGui::PopItemWidth();
                ImGui::Columns(1);

                ImGui::PopID();
            });
            if (remove)
                component->GetGameObject()->RemoveComponent<VWolf::ShapeRendererComponent>();
        }
    private:
        VWolf::MeshData GetMeshData() {
            if (strcmp(items[selection], "Box") == 0) {
                return VWolf::OBJLoader::Load("assets/basic_shapes/Box.obj", items[selection]);
            }
            else if (strcmp(items[selection], "Sphere") == 0) {
                return VWolf::OBJLoader::Load("assets/basic_shapes/Sphere.obj", items[selection]);
            }
            else if (strcmp(items[selection], "Geosphere") == 0) {
                return VWolf::OBJLoader::Load("assets/basic_shapes/Geosphere.obj", items[selection]);
            }
            else if (strcmp(items[selection], "Cylinder") == 0) {
                return VWolf::OBJLoader::Load("assets/basic_shapes/Cylinder.obj", items[selection]);
            }
            else if (strcmp(items[selection], "Grid") == 0) {
                return VWolf::OBJLoader::Load("assets/basic_shapes/Grid.obj", items[selection]);
            }
            else if (strcmp(items[selection], "Monkey") == 0) {
                return VWolf::OBJLoader::Load("assets/basic_shapes/Monkey.obj", items[selection]);
            }
            return VWolf::ShapeHelper::Create(items[selection]);
        }
    private:
        int selection = 0;
        std::array<const char*, 7> items = { "Box", "Sphere", "Geosphere", "Cylinder", "Grid", "Monkey", "Triangle" };
    };

    class CameraComponentInspector: public VWolf::ComponentInspector<VWolf::CameraComponent> {
    public:
        CameraComponentInspector() {}
        ~CameraComponentInspector() {}
    public:
        virtual void OnInspector(VWolf::CameraComponent* component) override {
            auto remove = DrawComponent<VWolf::CameraComponent>(component->GetName(), *component, [this](VWolf::CameraComponent& component) {
                ImGui::PushID("Camera");

                ImGui::PushID("Projection");
                ImGui::Columns(2);
                ImGui::SetColumnWidth(0, 100.0f);
                ImGui::Text("%s", "Projection");
                ImGui::NextColumn();

                ImGui::PushItemWidth(ImGui::CalcItemWidth());
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
                selection = component.IsOrthographic();
                if (ImGui::Combo("##Projection", &selection, items.data(), (int)items.size())) {
                    
                    component.SetOrthographic(selection == 1);
                }
                ImGui::PopStyleVar();
                ImGui::PopItemWidth();
                ImGui::Columns(1);
                ImGui::PopID();

                ImGui::PushID("Field of View");
                ImGui::Columns(2);
                ImGui::SetColumnWidth(0, 100.0f);
                ImGui::Text("%s", "Field of View");
                ImGui::NextColumn();

                ImGui::PushItemWidth(ImGui::CalcItemWidth());
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
                ImGui::SliderFloat("##FieldOfView", &component.GetFOV(), 30, 90);
                ImGui::PopStyleVar();
                ImGui::PopItemWidth();
                ImGui::Columns(1);
                ImGui::PopID();

                ImGui::PushID("Near clip");
                ImGui::Columns(2);
                ImGui::SetColumnWidth(0, 100.0f);
                ImGui::Text("%s", "Near clip");
                ImGui::NextColumn();

                ImGui::PushItemWidth(ImGui::CalcItemWidth());
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
                ImGui::DragFloat("##NearClip", &component.GetNearClip(), 0.01f, 0.01f, 1, "%.2f");
                ImGui::PopStyleVar();
                ImGui::PopItemWidth();
                ImGui::Columns(1);
                ImGui::PopID();

                ImGui::PushID("Far clip");
                ImGui::Columns(2);
                ImGui::SetColumnWidth(0, 100.0f);
                ImGui::Text("%s", "Far clip");
                ImGui::NextColumn();

                ImGui::PushItemWidth(ImGui::CalcItemWidth());
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
                ImGui::DragFloat("##FarClip", &component.GetFarClip(), 1, 10000, 1, "%.2f");
                ImGui::PopStyleVar();
                ImGui::PopItemWidth();
                ImGui::Columns(1);
                ImGui::PopID();

                ImGui::PushID("Zoom");
                ImGui::Columns(2);
                ImGui::SetColumnWidth(0, 100.0f);
                ImGui::Text("%s", "Zoom");
                ImGui::NextColumn();

                ImGui::PushItemWidth(ImGui::CalcItemWidth());
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
                ImGui::DragFloat("##Zoom", &component.GetZoom(), 1, 10000, 1, "%.2f");
                ImGui::PopStyleVar();
                ImGui::PopItemWidth();
                ImGui::Columns(1);
                ImGui::PopID();

                ImGui::PopID();
            });
            if (remove) {
                component->GetGameObject()->RemoveComponent<VWolf::CameraComponent>();
            }
        }

        std::array<const char*, 2> items = { "Perspective", "Ortographic" };
        int selection = 0;
    };

    class RigidBodyComponentInspector: public VWolf::ComponentInspector<VWolf::RigidBodyComponent> {
    public:
        RigidBodyComponentInspector() {}
        ~RigidBodyComponentInspector() {}
    public:
        virtual void OnInspector(VWolf::RigidBodyComponent* component) override {
            auto remove = DrawComponent<VWolf::RigidBodyComponent>(component->GetName(), *component, [this](VWolf::RigidBodyComponent& component) {
                ImGui::PushID("Camera");

                ImGui::PushID("Mass");
                ImGui::Columns(2);
                ImGui::SetColumnWidth(0, 100.0f);
                ImGui::Text("%s", "Mass");
                ImGui::NextColumn();

                ImGui::PushItemWidth(ImGui::CalcItemWidth());
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
                ImGui::DragFloat("##Mass", &component.GetMass(), 1, 1000000, 1, "%.2f");
                ImGui::PopStyleVar();
                ImGui::PopItemWidth();
                ImGui::Columns(1);
                ImGui::PopID();

                ImGui::PushID("Drag");
                ImGui::Columns(2);
                ImGui::SetColumnWidth(0, 100.0f);
                ImGui::Text("%s", "Drag");
                ImGui::NextColumn();

                ImGui::PushItemWidth(ImGui::CalcItemWidth());
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
                ImGui::DragFloat("##Drag", &component.GetDrag(), 0, 1000000, 1, "%.2f");
                ImGui::PopStyleVar();
                ImGui::PopItemWidth();
                ImGui::Columns(1);
                ImGui::PopID();

                ImGui::PushID("AngularDrag");
                ImGui::Columns(2);
                ImGui::SetColumnWidth(0, 100.0f);
                ImGui::Text("%s", "Angular Drag");
                ImGui::NextColumn();

                ImGui::PushItemWidth(ImGui::CalcItemWidth());
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
                ImGui::DragFloat("##AngularDrag", &component.GetAngularDrag(), 0, 1000000, 1, "%.2f");
                ImGui::PopStyleVar();
                ImGui::PopItemWidth();
                ImGui::Columns(1);
                ImGui::PopID();

                ImGui::PushID("UseGravity");
                ImGui::Columns(2);
                ImGui::SetColumnWidth(0, 100.0f);
                ImGui::Text("%s", "Use Gravity");
                ImGui::NextColumn();

                ImGui::PushItemWidth(ImGui::CalcItemWidth());
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
                ImGui::Checkbox("##UseGravity", &component.GetUseGravity());
                ImGui::PopStyleVar();
                ImGui::PopItemWidth();
                ImGui::Columns(1);
                ImGui::PopID();

                ImGui::PushID("BodyType");
                ImGui::Columns(2);
                ImGui::SetColumnWidth(0, 100.0f);
                ImGui::Text("%s", "Body Type");
                ImGui::NextColumn();

                ImGui::PushItemWidth(ImGui::CalcItemWidth());
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
                selection = component.GetBodyType();
                if (ImGui::Combo("##BodyType", &selection, items.data(), (int)items.size())) {                    
                    component.SetBodyType(selection);
                }
                ImGui::PopStyleVar();
                ImGui::PopItemWidth();
                ImGui::Columns(1);
                ImGui::PopID();

                ImGui::PopID();
            });
            if (remove)
                component->GetGameObject()->RemoveComponent<VWolf::RigidBodyComponent>();
        }

        std::array<const char*, 3> items = { "Static", "Kinematic", "Dynamic" };
        int selection = 0;
    };

    class MeshColliderComponentInspector: public VWolf::ComponentInspector<VWolf::MeshColliderComponent> {
    public:
        MeshColliderComponentInspector() {}
        ~MeshColliderComponentInspector() {}
    public:
        virtual void OnInspector(VWolf::MeshColliderComponent* component) override {
            auto remove = DrawComponent<VWolf::MeshColliderComponent>(component->GetName(), *component, [this](VWolf::MeshColliderComponent& component) {
                
            });
            if (remove)
                component->GetGameObject()->RemoveComponent<VWolf::MeshColliderComponent>();
        }
    };

    class SphereColliderComponentInspector: public VWolf::ComponentInspector<VWolf::SphereColliderComponent> {
    public:
        SphereColliderComponentInspector() {}
        ~SphereColliderComponentInspector() {}
    public:
        virtual void OnInspector(VWolf::SphereColliderComponent* component) override {
            auto remove = DrawComponent<VWolf::SphereColliderComponent>(component->GetName(), *component, [this](VWolf::SphereColliderComponent& component) {
                ImGui::PushID("Sphere Collider");

                ImGui::PushID("Radius");
                ImGui::Columns(2);
                ImGui::SetColumnWidth(0, 100.0f);
                ImGui::Text("%s", "Radius");
                ImGui::NextColumn();

                ImGui::PushItemWidth(ImGui::CalcItemWidth());
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
                ImGui::DragFloat("##Radius", &component.GetRadius(), 1, 1000000, 1, "%.2f");
                ImGui::PopStyleVar();
                ImGui::PopItemWidth();
                ImGui::Columns(1);
                ImGui::PopID();

                ImGui::PopID();
            });
            if (remove)
                component->GetGameObject()->RemoveComponent<VWolf::SphereColliderComponent>();
        }
    };

    class BoxColliderComponentInspector: public VWolf::ComponentInspector<VWolf::BoxColliderComponent> {
    public:
        BoxColliderComponentInspector() {}
        ~BoxColliderComponentInspector() {}
    public:
        virtual void OnInspector(VWolf::BoxColliderComponent* component) override {
            auto remove = DrawComponent<VWolf::BoxColliderComponent>(component->GetName(), *component, [this](VWolf::BoxColliderComponent& component) {
                
            });
            if (remove)
                component->GetGameObject()->RemoveComponent<VWolf::BoxColliderComponent>();
        }
    };

    class AudioListenerComponentInspector: public VWolf::ComponentInspector<VWolf::AudioListenerComponent> {
    public:
        AudioListenerComponentInspector() {}
        ~AudioListenerComponentInspector() {}
    public:
        virtual void OnInspector(VWolf::AudioListenerComponent* component) override {
            auto remove = DrawComponent<VWolf::AudioListenerComponent>(component->GetName(), *component, [this](VWolf::AudioListenerComponent& component) {
                
            });
            if (remove)
                component->GetGameObject()->RemoveComponent<VWolf::AudioListenerComponent>();
        }
    };

    class AudioSourceComponentInspector: public VWolf::ComponentInspector<VWolf::AudioSourceComponent> {
    public:
        AudioSourceComponentInspector() {}
        ~AudioSourceComponentInspector() {}
    public:
        virtual void OnInspector(VWolf::AudioSourceComponent* component) override {
            auto remove = DrawComponent<VWolf::AudioSourceComponent>(component->GetName(), *component, [this](VWolf::AudioSourceComponent& component) {
                ImGui::PushID("Audio Source");

                ImGui::PushID("File");
                ImGui::Columns(3);
                ImGui::SetColumnWidth(0, 100.0f);
                ImGui::Text("%s", "Audio File");
                ImGui::NextColumn();

                ImGui::PushItemWidth(ImGui::CalcItemWidth());
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
                ImGui::Text("%s", component.GetAudioFile().filename().c_str());
                ImGui::PopStyleVar();
                ImGui::NextColumn();
                float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
                if (ImGui::Button("...", ImVec2{ lineHeight, lineHeight }))
                {
                    ContainerView::GetMainView()
                    ->AddView(new ObjectExplorer(Extension::GetExtension("Audio"), [this, &component](auto path){
                        component.SetAudioFile(path);
                    }));
                }

                ImGui::Columns(1);
                ImGui::PopID();


                ImGui::PushID("Loop");
                ImGui::Columns(2);
                ImGui::SetColumnWidth(0, 100.0f);
                ImGui::Text("%s", "Loop");
                ImGui::NextColumn();

                ImGui::PushItemWidth(ImGui::CalcItemWidth());
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
                ImGui::Checkbox("##Loop", &component.GetLoop());
                ImGui::PopStyleVar();
                ImGui::PopItemWidth();
                ImGui::Columns(1);
                ImGui::PopID();

                ImGui::PopID();
            });
            if (remove)
                component->GetGameObject()->RemoveComponent<VWolf::AudioSourceComponent>();
        }
    };

    Inspector::Inspector(): View("Inspector") {
        VWolf::TransformComponent::SetComponentInspector(new TransformComponentInspector());
        VWolf::LightComponent::SetComponentInspector(new LightComponentInspector());
        VWolf::MeshFilterComponent::SetComponentInspector(new MeshFilterComponentInspector());
        VWolf::MeshRendererComponent::SetComponentInspector(new MeshRendererComponentInspector());
        VWolf::ShapeRendererComponent::SetComponentInspector(new ShapeRendererComponentInspector());
        VWolf::CameraComponent::SetComponentInspector(new CameraComponentInspector());
        VWolf::RigidBodyComponent::SetComponentInspector(new RigidBodyComponentInspector());
        VWolf::MeshColliderComponent::SetComponentInspector(new MeshColliderComponentInspector());
        VWolf::SphereColliderComponent::SetComponentInspector(new SphereColliderComponentInspector());
        VWolf::BoxColliderComponent::SetComponentInspector(new BoxColliderComponentInspector());
        VWolf::AudioListenerComponent::SetComponentInspector(new AudioListenerComponentInspector());
        VWolf::AudioSourceComponent::SetComponentInspector(new AudioSourceComponentInspector());
    }
    Inspector::~Inspector() {
        
    }

    void Inspector::SetInContainer() {
        GetContainer()->GetRoot()->Install(this, ImGuiDir_Right);
    }

    void Inspector::AfterSetInContainer() {
        ImGuiWindow* window = ImGui::FindWindowByName(title.c_str());
        if (window == NULL || window->DockNode == NULL || window->DockNode->TabBar == NULL)
            return;
        window->DockNode->TabBar->NextSelectedTabId = window->ID;
    }

    void Inspector::OnGui() {
        
        ImGui::Begin(title.c_str());
        if (this->gameObject) {
            if (ImGui::InputText("##Name", inputBuf, IM_ARRAYSIZE(inputBuf), ImGuiInputTextFlags_EnterReturnsTrue))
            {
                // In case of enter
                this->gameObject->SetName(inputBuf);
            }
            if (ImGui::GetID("##Name") == ImGui::GetActiveID()) {
                inputIsActive = true;
            }
            else if (inputIsActive) {
                inputIsActive = false;
                if (this->gameObject->GetName() != inputBuf) {
                    this->gameObject->SetName(inputBuf);
                }
            }
            ImGui::Separator();
            ImGui::PushItemWidth(-1);
            this->gameObject->OnInspector();
            ImGui::Separator();
            if (ButtonCenteredOnLine("Add Component")) {
                ImGui::OpenPopup("Component List");
            }
            ImGui::PopItemWidth();
            ImGui::Separator();
            DrawComponentList();
            DrawMaterial();
        }        
        ImGui::End();
    }

    void Inspector::DrawComponentList() {
        if (ImGui::BeginPopup("Component List", ImGuiPopupFlags_AnyPopupId))
        {
            if (ImGui::MenuItem("Light Component"))
            {
                if (!gameObject->HasComponent<VWolf::LightComponent>())
                    gameObject->AddComponent<VWolf::LightComponent>();
            }
            if (ImGui::MenuItem("Shape Renderer Component"))
            {
                if (!gameObject->HasComponent<VWolf::ShapeRendererComponent>())
                    gameObject->AddComponent<VWolf::ShapeRendererComponent>();
            }
            if (ImGui::MenuItem("Mesh Filter/Renderer Component"))
            {
                if (!gameObject->HasComponent<VWolf::MeshFilterComponent>())
                    gameObject->AddComponent<VWolf::MeshFilterComponent>();
                if (!gameObject->HasComponent<VWolf::MeshRendererComponent>())
                    gameObject->AddComponent<VWolf::MeshRendererComponent>();
            }
            if (ImGui::MenuItem("Camera Component"))
            {
                if (!gameObject->HasComponent<VWolf::CameraComponent>())
                    gameObject->AddComponent<VWolf::CameraComponent>();
            }
            if (ImGui::MenuItem("Rigid Body Component"))
            {
                if (!gameObject->HasComponent<VWolf::RigidBodyComponent>())
                    gameObject->AddComponent<VWolf::RigidBodyComponent>();
            }
            if (ImGui::MenuItem("Mesh Collider Component"))
            {
                if (!gameObject->HasComponent<VWolf::MeshColliderComponent>())
                    gameObject->AddComponent<VWolf::MeshColliderComponent>();
            }
            if (ImGui::MenuItem("Sphere Collider Component"))
            {
                if (!gameObject->HasComponent<VWolf::SphereColliderComponent>())
                    gameObject->AddComponent<VWolf::SphereColliderComponent>();
            }
            if (ImGui::MenuItem("Box Collider Component"))
            {
                if (!gameObject->HasComponent<VWolf::BoxColliderComponent>())
                    gameObject->AddComponent<VWolf::BoxColliderComponent>();
            }
            if (ImGui::MenuItem("Audio Listener Component"))
            {
                if (!gameObject->HasComponent<VWolf::AudioListenerComponent>())
                    gameObject->AddComponent<VWolf::AudioListenerComponent>();
            }
            if (ImGui::MenuItem("Audio Source Component"))
            {
                if (!gameObject->HasComponent<VWolf::AudioSourceComponent>())
                    gameObject->AddComponent<VWolf::AudioSourceComponent>();
            }

            ImGui::EndPopup();
        }
    }

    void Inspector::DrawMaterial(VWolf::Material& material) {
        bool isDefault = Defaults::Get()->IsDefault(material);
        ImGui::PushID("MaterialName");

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, 120.0f);
        ImGui::Text("%s", "Material");
        ImGui::NextColumn();
        ImGui::PushItemWidth(ImGui::CalcItemWidth());
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
        ImGui::Text("%s", material.GetName().c_str());
        ImGui::PopStyleVar();
        ImGui::PopItemWidth();
        ImGui::Columns(1);

        ImGui::PopID();
        if (isDefault)
            ImGui::BeginDisabled();
        for (auto property : material.GetProperties()) {
            switch (property.second) {
            case VWolf::ShaderDataType::Float4:
                ImGui::PushID(property.first.c_str());

                ImGui::Columns(2);
                ImGui::SetColumnWidth(0, 120.0f);
                ImGui::Text("%s", property.first.c_str());
                ImGui::NextColumn();

                ImGui::PushItemWidth(ImGui::CalcItemWidth());
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
                ImGui::ColorEdit4((std::string("##") + property.first + std::string("Selector")).c_str(),
                                  &material.GetColor(property.first).GetR(), ImGuiColorEditFlags_NoInputs);
                ImGui::PopStyleVar();
                ImGui::PopItemWidth();
                ImGui::Columns(1);

                ImGui::PopID();
                break;
            case VWolf::ShaderDataType::Float3:
                    DrawVec3Control(property.first, material.GetVector3(property.first));
                break;
            case VWolf::ShaderDataType::Float:
                ImGui::PushID(property.first.c_str());

                ImGui::Columns(2);
                ImGui::SetColumnWidth(0, 120.0f);
                ImGui::Text("%s", property.first.c_str());
                ImGui::NextColumn();

                ImGui::PushItemWidth(ImGui::CalcItemWidth());
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
                ImGui::SliderFloat((std::string("##") + property.first + std::string("Selector")).c_str(), &material.GetFloat(property.first), 0, 180);
                ImGui::PopStyleVar();
                ImGui::PopItemWidth();
                ImGui::Columns(1);

                ImGui::PopID();
                break;
            default: break;
            }
            
        }
        if (isDefault)
            ImGui::EndDisabled();
    }

    void Inspector::DrawMaterial() {
        // TODO: This is not a good way of getting the material
        if (this->gameObject && (this->gameObject->HasComponent<VWolf::ShapeRendererComponent>())) {
            auto& material = this->gameObject->GetComponent<VWolf::ShapeRendererComponent>().GetMaterial();
            DrawMaterial(material);
        } else if (this->gameObject && (this->gameObject->HasComponent<VWolf::MeshRendererComponent>())) {
            auto& material = this->gameObject->GetComponent<VWolf::MeshRendererComponent>().GetMaterial();
            DrawMaterial(material);
        }
    }

    void Inspector::SetGameObject(VWolf::Ref<VWolf::GameObject> gameObject) {
        this->gameObject = gameObject;
        if (gameObject)
            strcpy(inputBuf, gameObject->GetName().c_str());
    }
}
