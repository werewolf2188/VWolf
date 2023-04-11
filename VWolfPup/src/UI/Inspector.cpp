//
//  Inspector.cpp
//  VWolfPup
//
//  Created by Enrique Ricalde on 4/14/23.
//

#include "Inspector.h"

#include <imgui/imgui.h>

namespace VWolfPup {

    class TransformComponentInspector: public VWolf::ComponentInspector<VWolf::TransformComponent> {
    public:
        TransformComponentInspector() {}
        ~TransformComponentInspector() {}
    public:
        virtual void OnInspector(VWolf::TransformComponent* component) override {
            ImGui::LabelText(component->GetName().c_str(), "");

            ImGui::DragFloat3("Position", value_ptr(component->GetPosition()));
            ImGui::DragFloat3("Rotation", value_ptr(component->GetEulerAngles()));
            ImGui::DragFloat3("Scale", value_ptr(component->GetLocalScale()));
        }
    };

    class LightComponentInspector: public VWolf::ComponentInspector<VWolf::LightComponent> {
    public:
        LightComponentInspector() {}
        ~LightComponentInspector() {}
    public:
        virtual void OnInspector(VWolf::LightComponent* component) override {
            ImGui::LabelText(component->GetName().c_str(), "");

            if (ImGui::RadioButton("Directional", component->GetLight().type == VWolf::Light::LightType::Directional)) {
                component->GetLight().type = VWolf::Light::LightType::Directional;
            }
            if (ImGui::RadioButton("Point", component->GetLight().type == VWolf::Light::LightType::Point)) {
                component->GetLight().type = VWolf::Light::LightType::Point;
            }
            if (ImGui::RadioButton("Spot", component->GetLight().type == VWolf::Light::LightType::Spot)) {
                component->GetLight().type = VWolf::Light::LightType::Spot;
            }
            if (ImGui::RadioButton("Unknown", component->GetLight().type == VWolf::Light::LightType::Unknown)) {
                component->GetLight().type = VWolf::Light::LightType::Unknown;
            }
            ImGui::ColorEdit4("Light Color", VWolf::value_ptr(component->GetLight().color));
            ImGui::DragFloat3("Light Strength", VWolf::value_ptr(component->GetLight().strength), 0.1f, 0, 1);
            if (component->GetLight().type == VWolf::Light::LightType::Spot) {
                ImGui::DragFloat("Light CutOff", &component->GetLight().cutOff, 0.1f, 0, VWolf::radians(90.0f));
                ImGui::DragFloat("Light Exponent", &component->GetLight().exponent, 0.1f, 0, 100);
            }
        }
    };

    class ShapeRendererComponent: public VWolf::ComponentInspector<VWolf::ShapeRendererComponent> {
    public:
        ShapeRendererComponent() {}
        ~ShapeRendererComponent() {}
    public:
        virtual void OnInspector(VWolf::ShapeRendererComponent* component) override {
            ImGui::LabelText(component->GetName().c_str(), "");

            ImGui::LabelText("Shape", "%s", component->GetData().GetName().c_str());

        }
    };

    Inspector::Inspector(): View("Inspector") {
        VWolf::TransformComponent::SetComponentInspector(new TransformComponentInspector());
        VWolf::LightComponent::SetComponentInspector(new LightComponentInspector());
        VWolf::ShapeRendererComponent::SetComponentInspector(new ShapeRendererComponent());
    }
    Inspector::~Inspector() {
        
    }

    void Inspector::OnGui() {
        ImGui::Begin(title.c_str());
        if (this->gameObject) {
            ImGui::LabelText(this->gameObject->GetName().c_str(), "");
            this->gameObject->OnInspector();
        }
        DrawMaterial();
        ImGui::End();
    }

    void Inspector::DrawMaterial() {
        // TODO: This is not a good way of getting the material
        if (this->gameObject && this->gameObject->HasComponent<VWolf::ShapeRendererComponent>()) {
            auto& material = this->gameObject->GetComponent<VWolf::ShapeRendererComponent>().GetMaterial();
            ImGui::LabelText(material.GetName().c_str(), "");
            for (auto property : material.GetProperties()) {
                switch (property.second) {
                case VWolf::ShaderDataType::Float4:
                    ImGui::ColorEdit4(property.first.c_str(), VWolf::value_ptr(material.GetColor(property.first)));
                    break;
                case VWolf::ShaderDataType::Float3:
                    ImGui::DragFloat3(property.first.c_str(), VWolf::value_ptr(material.GetVector3(property.first)));
                    break;
                case VWolf::ShaderDataType::Float:
                    ImGui::SliderFloat(property.first.c_str(), &material.GetFloat(property.first), 0, 180);
                    break;
                default: break;
                }
            }
        }
    }

    void Inspector::SetGameObject(VWolf::Ref<VWolf::GameObject> gameObject) {
        this->gameObject = gameObject;
    }
}
