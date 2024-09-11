//
//  RigidBodyComponent.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 6/30/24.
//

#include "vwpch.h"
#include "RigidBodyComponent.h"
#include "TransformComponent.h"
#include "VWolf/Core/Math/VMath.h"

#include "VWolf/Core/Physics/Physics.h"
#include "GameObject.h"

namespace VWolf {
    RigidBodyComponent::RigidBodyComponent(): Component("RigidBody") {

    }

    RigidBodyComponent::RigidBodyComponent(RigidBodyComponent& rigidBody): Component("RigidBody") {
        this->mMass = rigidBody.mMass;
        this->mDrag = rigidBody.mDrag;
        this->mAngularDrag = rigidBody.mAngularDrag;
        this->mUseGravity = rigidBody.mUseGravity;
        this->mBodyType = rigidBody.mBodyType;
    }

    RigidBodyComponent::RigidBodyComponent(RigidBodyComponent&& rigidBody): Component("RigidBody") {
        this->mMass = rigidBody.mMass;
        this->mDrag = rigidBody.mDrag;
        this->mAngularDrag = rigidBody.mAngularDrag;
        this->mUseGravity = rigidBody.mUseGravity;
        this->mBodyType = mUseGravity;

        rigidBody.mMass = 0;
        rigidBody.mDrag = 0;
        rigidBody.mAngularDrag = 0;
        rigidBody.mUseGravity = false;
        rigidBody.mUseGravity = 0;
    }

    RigidBodyComponent::~RigidBodyComponent() {

    }

    void RigidBodyComponent::OnInspector() {
        RigidBodyComponent::componentInspector->OnInspector(this);
    }

    Component* RigidBodyComponent::Copy(entt::entity& handle, entt::registry& registry) {
        RigidBodyComponent& component = registry.emplace<RigidBodyComponent>(handle, *this);
        return &component;
    }

    void RigidBodyComponent::CreateRigidBody(reactphysics3d::PhysicsWorld* world, TransformComponent& component) {
        reactphysics3d::Vector3 position(component.GetPosition().GetX(), component.GetPosition().GetY(), component.GetPosition().GetZ());
        reactphysics3d::Quaternion orientation = reactphysics3d::Quaternion::fromEulerAngles({ radians(component.GetEulerAngles().GetX()), radians(component.GetEulerAngles().GetY()), radians(component.GetEulerAngles().GetZ()) });
        oldTransform = new reactphysics3d::Transform(position, orientation);
        rigidBody = world->createRigidBody(*oldTransform);
        GetGameObject()->SetRigidBody(rigidBody);
        rigidBody->setMass(mMass);
        rigidBody->setLinearDamping(mDrag);
        rigidBody->setAngularDamping(mAngularDrag);
        rigidBody->enableGravity(mUseGravity);
        rigidBody->setType((reactphysics3d::BodyType)mBodyType);
        rigidBody->setIsDebugEnabled(true);
    }

    void RigidBodyComponent::Update(TransformComponent& component, float factor) {
        const reactphysics3d::Transform& transform = rigidBody->getTransform();
//        const reactphysics3d::Transform transform = reactphysics3d::Transform::interpolateTransforms(*oldTransform, newTransform, factor);
        const reactphysics3d::Vector3& position = transform.getPosition();
        const reactphysics3d::Quaternion& quaternion = transform.getOrientation();
        Quaternion quat(quaternion.w, quaternion.x, quaternion.y, quaternion.z);
        Vector3 euler = quat.EulerAngles().Degrees();
//        Quat quat(transform.getOrientation().x, transform.getOrientation().y, transform.getOrientation().z, transform.getOrientation().w);
        component.SetPosition({ position.x, position.y, position.z });
//        component.SetEulerAngles(eulerAngles(quat));
        component.SetEulerAngles(euler);
//        oldTransform->setPosition(transform.getPosition());
//        oldTransform->setOrientation(transform.getOrientation());
    }

    void RigidBodyComponent::DestroyRigidBody(reactphysics3d::PhysicsWorld* world) {
        world->destroyRigidBody(rigidBody);
        rigidBody = nullptr;
    }

    VWOLF_COMPONENT_INSPECTOR_IMPLEMENTATION(RigidBodyComponent);
}
