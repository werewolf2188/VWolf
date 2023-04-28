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

static void DrawVec3Control(const std::string& label, VWolf::Vector3Float& values, float resetValue = 0.0f, float columnWidth = 100.0f)
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
        values.x = resetValue;
    ImGui::PopFont();
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
    ImGui::PopItemWidth();
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
    ImGui::PushFont(boldFont);
    if (ImGui::Button("Y", buttonSize))
        values.y = resetValue;
    ImGui::PopFont();
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
    ImGui::PopItemWidth();
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
    ImGui::PushFont(boldFont);
    if (ImGui::Button("Z", buttonSize))
        values.z = resetValue;
    ImGui::PopFont();
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
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
                
                ImGui::ColorEdit4("##Light Color", VWolf::value_ptr(component.GetLight().color), ImGuiColorEditFlags_NoInputs);
                
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

                ImGui::DragFloat3("##Light Strength", VWolf::value_ptr(component.GetLight().strength), 0.1f, 0, 1);
                
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
                    
                    component.SetData(VWolf::ShapeHelper::Create(items[selection]));
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
        int selection = 0;
        std::array<const char*, 6> items = { "Box", "Sphere", "Geosphere", "Cylinder", "Grid", "Triangle" };
    };

    Inspector::Inspector(): View("Inspector") {
        VWolf::TransformComponent::SetComponentInspector(new TransformComponentInspector());
        VWolf::LightComponent::SetComponentInspector(new LightComponentInspector());
        VWolf::ShapeRendererComponent::SetComponentInspector(new ShapeRendererComponentInspector());
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

            ImGui::EndPopup();
        }
    }

    void Inspector::DrawMaterial() {
        // TODO: This is not a good way of getting the material
        if (this->gameObject && this->gameObject->HasComponent<VWolf::ShapeRendererComponent>()) {
            auto& material = this->gameObject->GetComponent<VWolf::ShapeRendererComponent>().GetMaterial();
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
                                      VWolf::value_ptr(material.GetColor(property.first)), ImGuiColorEditFlags_NoInputs);
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
        }
    }

    void Inspector::SetGameObject(VWolf::Ref<VWolf::GameObject> gameObject) {
        this->gameObject = gameObject;
        strcpy(inputBuf, gameObject->GetName().c_str());
    }
}
