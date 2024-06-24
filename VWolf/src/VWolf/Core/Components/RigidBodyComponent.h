//
//  RigidBodyComponent.h
//  VWolf
//
//  Created by Enrique Ricalde on 6/30/24.
//

#pragma once

#include "BaseComponent.h"

namespace reactphysics3d {
    class RigidBody;
    class PhysicsWorld;
    class Transform;
}

namespace VWolf {
    class TransformComponent;

    class RigidBodyComponent: public Component {
    public:
        RigidBodyComponent();
        RigidBodyComponent(RigidBodyComponent& rigidBody);
        RigidBodyComponent(RigidBodyComponent&& rigidBody);
        ~RigidBodyComponent();
    public:
        virtual void OnInspector() override;
        virtual Component* Copy(entt::entity& handle, entt::registry& registry) override;
    public:
        void CreateRigidBody(reactphysics3d::PhysicsWorld* world, TransformComponent& component);
        void Update(TransformComponent& component, float factor);
        void DestroyRigidBody(reactphysics3d::PhysicsWorld* world);
    public:
        float& GetMass() { return mMass; }
        float GetMass() const { return mMass; }
        void SetMass(float mass) { mMass = mass; }
        float& GetDrag() { return mDrag; }
        float GetDrag() const { return mDrag; }
        void SetDrag(float drag) { mDrag = drag; }
        float& GetAngularDrag() { return mAngularDrag; }
        float GetAngularDrag() const { return mAngularDrag; }
        void SetAngularDrag(float angularDrag) { mAngularDrag = angularDrag; }
        bool& GetUseGravity() { return mUseGravity; }
        bool GetUseGravity() const { return mUseGravity; }
        void SetUseGravity(float useGravity) { mUseGravity = useGravity; }
        uint32_t& GetBodyType() { return mBodyType; }
        uint32_t GetBodyType() const { return mBodyType; }
        void SetBodyType(uint32_t bodyType) { mBodyType = bodyType; }
        
    private:
        reactphysics3d::RigidBody* rigidBody;
        reactphysics3d::Transform* oldTransform;

        float mMass = 1;
        float mDrag = 0;
        float mAngularDrag = 0;
        bool mUseGravity = true;
        uint32_t mBodyType = 2; // enum class BodyType {STATIC, KINEMATIC, DYNAMIC};
        
        VWOLF_COMPONENT_INSPECTOR_DEFINE(RigidBodyComponent);
    };
}
