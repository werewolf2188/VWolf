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
        position = Vector3Float(0, 0, 0);
        eulerAngles = Vector3Float(0, 0, 0);
        localScale = Vector3Float(1, 1, 1);
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

        transform.position = Vector3Float(0, 0, 0);
        transform.eulerAngles = Vector3Float(0, 0, 0);
        transform.localScale = Vector3Float(1, 1, 1);
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
        matrix = translate(MatrixFloat4x4(1.0f), position);
        matrix = rotate(matrix, radians(eulerAngles.x), { 1.0f, 0.0f, 0.0f });
        matrix = rotate(matrix, radians(eulerAngles.y), { 0.0f, 1.0f, 0.0f });
        matrix = rotate(matrix, radians(eulerAngles.z), { 0.0f, 0.0f, 1.0f });
        matrix = scale(matrix, localScale);
    }

    void TransformComponent::OnInspector() {
        TransformComponent::componentInspector->OnInspector(this);
    }

    VWOLF_COMPONENT_INSPECTOR_IMPLEMENTATION(TransformComponent);
}
