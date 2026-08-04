//
//  TransformComponent.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 4/14/23.
//

#include "vwpch.h"
#include "TransformComponent.h"

namespace VWolf {
    TransformComponent::TransformComponent():
    Component(ClassNameCleaner::Current().GetClassName<TransformComponent>()) {
        position = Vector3(0, 0, 0);
        eulerAngles = Vector3(0, 0, 0);
        localScale = Vector3(1, 1, 1);
    }

    TransformComponent::TransformComponent(const TransformComponent& transform):
    Component(ClassNameCleaner::Current().GetClassName<TransformComponent>(), transform.id) {
        this->position = transform.position;
        this->eulerAngles = transform.eulerAngles;
        this->localScale = transform.localScale;
        this->SetGameObject(const_cast<TransformComponent&>(transform).GetGameObject());
    }

    TransformComponent::TransformComponent(TransformComponent&& transform):
    Component(ClassNameCleaner::Current().GetClassName<TransformComponent>(), transform.id) {
        this->position = transform.position;
        this->eulerAngles = transform.eulerAngles;
        this->localScale = transform.localScale;
        this->SetGameObject(transform.GetGameObject());

        transform.position = Vector3(0, 0, 0);
        transform.eulerAngles = Vector3(0, 0, 0);
        transform.localScale = Vector3(1, 1, 1);
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

    Ref<Component> TransformComponent::Copy(entt::entity& handle, entt::registry& registry) {
        Ref<TransformComponent> component = CopyComponent<TransformComponent>(handle, registry);
        return component;
    }

    VWOLF_CREATE_CONVERT_GENERIC_CLASS_ENCODER_WITH_NAME(TransformComponent, "TransformComponent")
}
