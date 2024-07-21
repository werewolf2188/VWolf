//
//  TransformComponent.hpp
//  VWolf
//
//  Created by Enrique Ricalde on 4/14/23.
//

#pragma once

#include "BaseComponent.h"
#include "VWolf/Core/Math/VMath.h"

namespace VWolf {

    class TransformComponent: public Component {
    public:
        TransformComponent();
        TransformComponent(TransformComponent& transform);
        TransformComponent(TransformComponent&& transform);
        ~TransformComponent();
    public:
        Vector3Float& GetPosition() { return position; }
        void SetPosition(Vector3Float position) { this->position = position; }
        // TODO: Pass this to quaternion
        Vector3Float& GetEulerAngles() { return eulerAngles; }
        void SetEulerAngles(Vector3Float eulerAngles) { this->eulerAngles = eulerAngles; }
        Vector3Float& GetLocalScale() { return localScale; }
        void SetLocalScale(Vector3Float localScale) { this->localScale = localScale; }

        MatrixFloat4x4& GetWorldMatrix() {
            return matrix;
        }

        void Apply();
    public:
        virtual void OnInspector() override;
        virtual Component* Copy(entt::entity& handle, entt::registry& registry) override;
    public:
        TransformComponent& operator=(TransformComponent t);
    private:
        Vector3Float position;
        Vector3Float eulerAngles;
        Vector3Float localScale;
        MatrixFloat4x4 matrix;
    VWOLF_COMPONENT_INSPECTOR_DEFINE(TransformComponent);
    };
}
