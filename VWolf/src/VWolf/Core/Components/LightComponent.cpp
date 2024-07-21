//
//  LightComponent.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 4/14/23.
//

#include "vwpch.h"
#include "TransformComponent.h"
#include "LightComponent.h"

namespace VWolf {
    LightComponent::LightComponent(): Component("Light") {
        // TODO: Testing
        light.color = { 1.0f, 1.0f, 1.0f, 1.0f };
        light.strength = { 0.5f, 0.5f, 0.5f, 0.5f };
        light.cutOff = radians(15.0f);
        light.type = Light::LightType::Point;
        light.exponent = 50.0f;
        light.position = { 0.0f, 0.0f, 0.0f, 1.0f };
        light.direction = { 0.0f, 0.0f, 0.0f, 0.0f };
    }

    LightComponent::LightComponent(LightComponent& light): Component("Light"), light(light.light) {}

    LightComponent::LightComponent(LightComponent&& light): Component("Light"), light(std::move(light.light)) {}

    LightComponent::~LightComponent() {}

    LightComponent& LightComponent::operator=(LightComponent& light) {
        this->light = light.light;
        return *this;
    }

    Light& LightComponent::GetLight(TransformComponent component) {
        return light;
    }

    void LightComponent::OnInspector() {
        LightComponent::componentInspector->OnInspector(this);
    }

    Component* LightComponent::Copy(entt::entity& handle, entt::registry& registry) {
        LightComponent& component = registry.emplace<LightComponent>(handle, *this);
        return &component;
    }

    VWOLF_COMPONENT_INSPECTOR_IMPLEMENTATION(LightComponent);
}
