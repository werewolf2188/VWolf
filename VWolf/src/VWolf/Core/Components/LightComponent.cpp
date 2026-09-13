//
//  LightComponent.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 4/14/23.
//

#include "vwpch.h"
#include "TransformComponent.h"
#include "LightComponent.h"
#include "GameObject.h"

namespace VWolf {
    LightComponent::LightComponent(): Component(ClassNameCleaner::Current().GetClassName<LightComponent>()) {
        // TODO: Testing
        light.color = { 1.0f, 1.0f, 1.0f, 1.0f };
        light.strength = { 0.5f, 0.5f, 0.5f, 0.5f };
        light.cutOff = Mathf::Deg2Rad * 15.0f;
        light.type = Light::LightType::Point;
        light.exponent = 50.0f;
        light.position = { 0.0f, 0.0f, 0.0f, 1.0f };
        light.direction = { 0.0f, 0.0f, 0.0f, 0.0f };
    }

    LightComponent::LightComponent(const LightComponent& light):
    Component(light), light(light.light) {}

    LightComponent::LightComponent(LightComponent&& light):
    Component(ClassNameCleaner::Current().GetClassName<LightComponent>(), light.id), light(std::move(light.light)) {}

    LightComponent::~LightComponent() {}

    LightComponent& LightComponent::operator=(const LightComponent& light) {
        Component::operator=(light);
        this->light = light.light;
        return *this;
    }

    Ref<Component> LightComponent::Copy(entt::entity& handle, entt::registry& registry) {
        Ref<LightComponent> component = CopyComponent<LightComponent>(handle, registry);
        return component;
    }

    Light& LightComponent::GetLight_Ex() {
        TransformComponent& transform = GetGameObject()->GetTransform();
        light.position = transform.GetPosition();
        // TODO: This should come from the transform
        Vector3 direction = transform.GetEulerAngles();
        Quaternion orientation(direction * Mathf::Deg2Rad);
        
        
        light.direction = orientation.GetOrientation({ 0.0f, 0.0f, 1.0f }).Normalized();
        
        return light;
    }

    VWOLF_CREATE_CONVERT_GENERIC_CLASS_ENCODER_WITH_NAME(LightComponent, "LightComponent")
}
