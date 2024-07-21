//
//  BoxColliderComponent.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 7/20/24.
//

#include "vwpch.h"
#include "BoxColliderComponent.h"
#include "TransformComponent.h"
#include "VWolf/Core/Math/VMath.h"

#include "VWolf/Core/Physics/Physics.h"
#include "GameObject.h"

namespace VWolf {

    BoxColliderComponent::BoxColliderComponent(): Component("BoxCollider") {}

    BoxColliderComponent::BoxColliderComponent(BoxColliderComponent& sphereCollider): Component("BoxCollider") {
    }

    BoxColliderComponent::BoxColliderComponent(BoxColliderComponent&& sphereCollider): Component("BoxCollider") {
    }

    BoxColliderComponent::~BoxColliderComponent() {}

    void BoxColliderComponent::OnInspector() {
        BoxColliderComponent::componentInspector->OnInspector(this);
    }

    Component* BoxColliderComponent::Copy(entt::entity& handle, entt::registry& registry) {
        BoxColliderComponent& component = registry.emplace<BoxColliderComponent>(handle, *this);
        return &component;
    }

    void BoxColliderComponent::CreateBoxCollider(MeshData& data, TransformComponent& component) {
        boxShape = Physics::GetCommon().createBoxShape({ component.GetLocalScale().x, component.GetLocalScale().y, component.GetLocalScale().z });
        scale = component.GetLocalScale();
        reactphysics3d::Transform transform = reactphysics3d::Transform::identity();
        reactphysics3d::RigidBody* rigidBody = GetGameObject()->GetRigidBody();
        if (rigidBody != nullptr) {
            collider = rigidBody->addCollider(boxShape, transform);
        }
    }

    void BoxColliderComponent::Update(TransformComponent& component) {
        if (boxShape != nullptr && component.GetLocalScale() != scale) {
            boxShape->setHalfExtents({ component.GetLocalScale().x, component.GetLocalScale().y, component.GetLocalScale().z });
            scale = component.GetLocalScale();
        }
    }

    void BoxColliderComponent::Destroy() {
        if (collider != nullptr) {
            GetGameObject()->GetRigidBody()->removeCollider(collider);
        }
        if (boxShape != nullptr) {
            Physics::GetCommon().destroyBoxShape(boxShape);
        }
    }

    VWOLF_COMPONENT_INSPECTOR_IMPLEMENTATION(BoxColliderComponent);
}

