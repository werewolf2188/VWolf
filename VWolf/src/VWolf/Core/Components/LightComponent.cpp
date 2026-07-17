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

    LightComponent::LightComponent(LightComponent& light):
    Component(ClassNameCleaner::Current().GetClassName<LightComponent>(), light.id), light(light.light) {}

    LightComponent::LightComponent(LightComponent&& light):
    Component(ClassNameCleaner::Current().GetClassName<LightComponent>(), light.id), light(std::move(light.light)) {}

    LightComponent::~LightComponent() {}

    LightComponent& LightComponent::operator=(const LightComponent& light) {
        this->light = light.light;
        return *this;
    }

    Light& LightComponent::GetLight(TransformComponent component) {
        return light;
    }

    Component* LightComponent::Copy(entt::entity& handle, entt::registry& registry) {
        LightComponent& component = registry.emplace<LightComponent>(handle, *this);
        return &component;
    }

    VWOLF_CREATE_CONVERT_GENERIC_CLASS_ENCODER_WITH_NAME(LightComponent, "LightComponent")
}
