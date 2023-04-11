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
