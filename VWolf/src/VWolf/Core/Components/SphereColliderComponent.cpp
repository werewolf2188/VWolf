//
//  SphereColliderComponent.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 7/6/24.
//

#include "vwpch.h"
#include "SphereColliderComponent.h"
#include "TransformComponent.h"
#include "VWolf/Core/Math/VMath.h"

#include "VWolf/Core/Physics/Physics.h"
#include "GameObject.h"

namespace VWolf {

    SphereColliderComponent::SphereColliderComponent(): Component("SphereCollider") {}

    SphereColliderComponent::SphereColliderComponent(SphereColliderComponent& sphereCollider): Component("SphereCollider") {
        this->mRadius = sphereCollider.mRadius;
    }

    SphereColliderComponent::SphereColliderComponent(SphereColliderComponent&& sphereCollider): Component("SphereCollider") {
        this->mRadius = sphereCollider.mRadius;

        sphereCollider.mRadius = 0;
    }

    SphereColliderComponent::~SphereColliderComponent() {}

    void SphereColliderComponent::OnInspector() {
        SphereColliderComponent::componentInspector->OnInspector(this);
    }

    Component* SphereColliderComponent::Copy(entt::entity& handle, entt::registry& registry) {
        SphereColliderComponent& component = registry.emplace<SphereColliderComponent>(handle, *this);
        return &component;
    }

    void SphereColliderComponent::CreateSphereCollider(MeshData& data, TransformComponent& component) {
        mRadius = std::max(std::max(component.GetLocalScale().GetX(), component.GetLocalScale().GetY()), component.GetLocalScale().GetZ());
        sphereShape = Physics::GetCommon().createSphereShape(mRadius);
        reactphysics3d::Transform transform = reactphysics3d::Transform::identity();
        reactphysics3d::RigidBody* rigidBody = GetGameObject()->GetRigidBody();
        if (rigidBody != nullptr) {
            collider = rigidBody->addCollider(sphereShape, transform);
        }
    }

    void SphereColliderComponent::Update(TransformComponent& component) {
        float currentRadius = std::max(std::max(component.GetLocalScale().GetX(), component.GetLocalScale().GetY()), component.GetLocalScale().GetZ());
        if (sphereShape != nullptr && currentRadius != mRadius) {
            sphereShape->setRadius(currentRadius);
            mRadius = currentRadius;
        }
    }

    void SphereColliderComponent::Destroy() {
        if (collider != nullptr) {
            GetGameObject()->GetRigidBody()->removeCollider(collider);
        }
        if (sphereShape != nullptr) {
            Physics::GetCommon().destroySphereShape(sphereShape);
        }
    }

    VWOLF_COMPONENT_INSPECTOR_IMPLEMENTATION(SphereColliderComponent);
}
