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
        Vector3& GetPosition() { return position; }
        void SetPosition(Vector3 position) { this->position = position; }
        // TODO: Pass this to quaternion
        Vector3& GetEulerAngles() { return eulerAngles; }
        void SetEulerAngles(Vector3 eulerAngles) { this->eulerAngles = eulerAngles; }
        Vector3& GetLocalScale() { return localScale; }
        void SetLocalScale(Vector3 localScale) { this->localScale = localScale; }

        Matrix4x4& GetWorldMatrix() {
            return matrix;
        }

        void Apply();
    public:
        virtual void OnInspector() override;
        virtual Component* Copy(entt::entity& handle, entt::registry& registry) override;
    public:
        TransformComponent& operator=(TransformComponent t);
    private:
        Vector3 position;
        Vector3 eulerAngles;
        Vector3 localScale;
        Matrix4x4 matrix;
    VWOLF_COMPONENT_INSPECTOR_DEFINE(TransformComponent);
    };
}
