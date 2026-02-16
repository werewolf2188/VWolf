//
//  TransformComponent.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 4/14/23.
//

#include "vwpch.h"
#include "TransformComponent.h"

namespace VWolf {
    TransformComponent::TransformComponent(): Component("Transform") {
        position = Vector3(0, 0, 0);
        eulerAngles = Vector3(0, 0, 0);
        localScale = Vector3(1, 1, 1);
    }

    TransformComponent::TransformComponent(TransformComponent& transform): Component("Transform") {
        this->position = transform.position;
        this->eulerAngles = transform.eulerAngles;
        this->localScale = transform.localScale;
        this->SetGameObject(transform.GetGameObject());
    }

    TransformComponent::TransformComponent(TransformComponent&& transform): Component("Transform") {
        this->position = transform.position;
        this->eulerAngles = transform.eulerAngles;
        this->localScale = transform.localScale;
        this->SetGameObject(transform.GetGameObject());

        transform.position = Vector3(0, 0, 0);
        transform.eulerAngles = Vector3(0, 0, 0);
        transform.localScale = Vector3(1, 1, 1);
        transform.SetGameObject(nullptr);
    }

    TransformComponent& TransformComponent::operator=(TransformComponent t) {
        this->position = t.position;
        this->eulerAngles = t.eulerAngles;
        this->localScale = t.localScale;
        this->SetGameObject(t.GetGameObject());
        return *this;
    }

    TransformComponent::~TransformComponent() {}

    void TransformComponent::Apply() {
        matrix = Matrix4x4::TRS(position,
                                Quaternion::Euler((Mathf::Deg2Rad * eulerAngles.GetX()),
                                                  (Mathf::Deg2Rad * eulerAngles.GetY()),
                                                  (Mathf::Deg2Rad * eulerAngles.GetZ())),
                                localScale);
    }

    void TransformComponent::OnInspector() {
        TransformComponent::componentInspector->OnInspector(this);
    }

    Component* TransformComponent::Copy(entt::entity& handle, entt::registry& registry) {
        TransformComponent& component = registry.emplace<TransformComponent>(handle, *this);
        return &component;
    }

    VWOLF_COMPONENT_INSPECTOR_IMPLEMENTATION(TransformComponent);
}
