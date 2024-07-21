//
//  SphereColliderComponent.h
//  VWolf
//
//  Created by Enrique Ricalde on 7/6/24.
//

#pragma once

#include "BaseComponent.h"
#include "VWolf/Core/Render/RenderStructs.h"

namespace reactphysics3d{
    class SphereShape;
    class Collider;
}

namespace VWolf {
    class TransformComponent;

    class SphereColliderComponent: public Component {
    public:
        SphereColliderComponent();
        SphereColliderComponent(SphereColliderComponent& sphereCollider);
        SphereColliderComponent(SphereColliderComponent&& sphereCollider);
        ~SphereColliderComponent();
    public:
        virtual void OnInspector() override;
        virtual Component* Copy(entt::entity& handle, entt::registry& registry) override;
    public:
        void CreateSphereCollider(MeshData& data, TransformComponent& component);
        void Update(TransformComponent& component);
        void Destroy();
    public:
        float GetRadius() const { return mRadius; }
        float& GetRadius() { return mRadius; }
        void SetRadius(float radius) { mRadius = radius; }
    private:
        reactphysics3d::SphereShape* sphereShape;
        reactphysics3d::Collider* collider;

        float mRadius = 1;

        VWOLF_COMPONENT_INSPECTOR_DEFINE(SphereColliderComponent);
    };
}
