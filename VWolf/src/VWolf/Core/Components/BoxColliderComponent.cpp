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

    BoxColliderComponent::BoxColliderComponent():
    Component(ClassNameCleaner::Current().GetClassName<BoxColliderComponent>()) {}

    BoxColliderComponent::BoxColliderComponent(const BoxColliderComponent& sphereCollider):
    Component(ClassNameCleaner::Current().GetClassName<BoxColliderComponent>(), sphereCollider.id) {
    }

    BoxColliderComponent::BoxColliderComponent(BoxColliderComponent&& sphereCollider):
    Component(ClassNameCleaner::Current().GetClassName<BoxColliderComponent>(), sphereCollider.id) {
    }

    BoxColliderComponent::~BoxColliderComponent() {}

    Ref<Component> BoxColliderComponent::Copy(entt::entity& handle, entt::registry& registry) {
        Ref<BoxColliderComponent> component = CopyComponent<BoxColliderComponent>(handle, registry);
        return component;
    }

    void BoxColliderComponent::CreateBoxCollider(Ref<Mesh> data, TransformComponent& component) {
        boxShape = Ref<reactphysics3d::BoxShape>(Physics::GetCommon().createBoxShape({ component.GetLocalScale().GetX(), component.GetLocalScale().GetY(), component.GetLocalScale().GetZ() }), [](reactphysics3d::BoxShape* shape){
            Physics::GetCommon().destroyBoxShape(shape);
        });
        scale = component.GetLocalScale();
        reactphysics3d::Transform transform = reactphysics3d::Transform::identity();
        reactphysics3d::RigidBody* rigidBody = GetGameObject()->GetRigidBody();
        if (rigidBody != nullptr) {
            collider = Ref<reactphysics3d::Collider>(rigidBody->addCollider(boxShape.get(), transform), [this](reactphysics3d::Collider* col) {
                GetGameObject()->GetRigidBody()->removeCollider(col);
            });
        }
    }

    void BoxColliderComponent::Update(TransformComponent& component) {
        if (boxShape != nullptr && component.GetLocalScale() != scale) {
            boxShape->setHalfExtents({ component.GetLocalScale().GetX(), component.GetLocalScale().GetY(), component.GetLocalScale().GetZ() });
            scale = component.GetLocalScale();
        }
    }

    void BoxColliderComponent::Destroy() {
        collider = nullptr;
        boxShape = nullptr;
    }

    VWOLF_CREATE_CONVERT_GENERIC_CLASS_ENCODER_WITH_NAME(BoxColliderComponent, "BoxColliderComponent")
}

