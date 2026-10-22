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
        RestoreQuaternionFromEulerAngles();
    }

    TransformComponent::TransformComponent(const TransformComponent& transform):
    Component(transform) {
        this->position = transform.position;
        this->eulerAngles = transform.eulerAngles;
        this->orientation = transform.orientation;
        this->localScale = transform.localScale;
        this->SetGameObject(const_cast<TransformComponent&>(transform).GetGameObject());
    }

    TransformComponent::TransformComponent(TransformComponent&& transform):
    Component(ClassNameCleaner::Current().GetClassName<TransformComponent>(), transform.id) {
        this->position = transform.position;
        this->eulerAngles = transform.eulerAngles;
        this->orientation = transform.orientation;
        this->localScale = transform.localScale;
        this->SetGameObject(transform.GetGameObject());

        transform.position = Vector3(0, 0, 0);
        transform.eulerAngles = Vector3(0, 0, 0);
        transform.localScale = Vector3(1, 1, 1);
        transform.RestoreQuaternionFromEulerAngles();
    }

    TransformComponent& TransformComponent::operator=(TransformComponent t) {
        Component::operator=(t);
        this->position = t.position;
        this->eulerAngles = t.eulerAngles;
        this->orientation = t.orientation;
        this->localScale = t.localScale;
        this->SetGameObject(t.GetGameObject());
        return *this;
    }

    TransformComponent::~TransformComponent() {}

    void TransformComponent::SetQuaternion(Quaternion quaternion) {
        orientation = quaternion;
        eulerAngles = quaternion.EulerAngles().Degrees();
    }

    void TransformComponent::RestoreQuaternionFromEulerAngles() {
        orientation = Quaternion::Euler(Mathf::Deg2Rad * eulerAngles.GetX(),
                                        Mathf::Deg2Rad * eulerAngles.GetY(),
                                        Mathf::Deg2Rad * eulerAngles.GetZ());
    }

    void TransformComponent::Apply() {
        matrix = Matrix4x4::TRS(position, orientation, localScale);
    }

    Ref<Component> TransformComponent::Copy(entt::entity& handle, entt::registry& registry) {
        Ref<TransformComponent> component = CopyComponent<TransformComponent>(handle, registry);
        return component;
    }

    Vector3 TransformComponent::Up() const {
        Quaternion rotation = orientation;
        return rotation.GetOrientation({ 0, 1, 0 });
    }

    Vector3 TransformComponent::Right() const {
        Quaternion rotation = orientation;
        return rotation.GetOrientation({ 1, 0, 0 });
    }

    Vector3 TransformComponent::Forward() const {
        Quaternion rotation = orientation;
        return rotation.GetOrientation({ 0, 0, 1 });
    }

    VWOLF_CREATE_CONVERT_GENERIC_CLASS_ENCODER_WITH_NAME(TransformComponent, "TransformComponent")
}
