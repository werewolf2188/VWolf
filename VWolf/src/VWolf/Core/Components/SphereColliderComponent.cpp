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

    SphereColliderComponent::SphereColliderComponent():
    Component(ClassNameCleaner::Current().GetClassName<SphereColliderComponent>()) {}

    SphereColliderComponent::SphereColliderComponent(const SphereColliderComponent& sphereCollider):
    Component(ClassNameCleaner::Current().GetClassName<SphereColliderComponent>(), sphereCollider.id) {
        this->mRadius = sphereCollider.mRadius;
    }

    SphereColliderComponent::SphereColliderComponent(SphereColliderComponent&& sphereCollider):
    Component(ClassNameCleaner::Current().GetClassName<SphereColliderComponent>(), sphereCollider.id) {
        this->mRadius = sphereCollider.mRadius;

        sphereCollider.mRadius = 0;
    }

    SphereColliderComponent::~SphereColliderComponent() {}

    Ref<Component> SphereColliderComponent::Copy(entt::entity& handle, entt::registry& registry) {
        Ref<SphereColliderComponent> component = CopyComponent<SphereColliderComponent>(handle, registry);
        return component;
    }

    void SphereColliderComponent::CreateSphereCollider(Ref<Mesh> data, TransformComponent& component) {
        mRadius = std::max(std::max(component.GetLocalScale().GetX(), component.GetLocalScale().GetY()), component.GetLocalScale().GetZ());
        sphereShape = Ref<reactphysics3d::SphereShape>(Physics::GetCommon().createSphereShape(mRadius), [](reactphysics3d::SphereShape* sphereShape) {
            Physics::GetCommon().destroySphereShape(sphereShape);
        });
        reactphysics3d::Transform transform = reactphysics3d::Transform::identity();
        reactphysics3d::RigidBody* rigidBody = GetGameObject()->GetRigidBody();
        if (rigidBody != nullptr) {
            collider = Ref<reactphysics3d::Collider>(rigidBody->addCollider(sphereShape.get(), transform), [this](reactphysics3d::Collider* collider) {
                GetGameObject()->GetRigidBody()->removeCollider(collider);
            });
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
        collider = nullptr;
        sphereShape = nullptr;
    }

    VWOLF_CREATE_CONVERT_GENERIC_CLASS_ENCODER_WITH_NAME(SphereColliderComponent, "SphereColliderComponent")
}
