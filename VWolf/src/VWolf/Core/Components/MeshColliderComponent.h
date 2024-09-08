//
//  MeshColliderComponent.hpp
//  VWolf
//
//  Created by Enrique Ricalde on 7/2/24.
//

#pragma once

#include "BaseComponent.h"
#include "VWolf/Core/Render/RenderStructs.h"

namespace reactphysics3d{
    class ConcaveMeshShape;
    class TriangleMesh;
    class Collider;
    class Transform;
}

namespace VWolf {
    class TransformComponent;

    class MeshColliderComponent: public Component {
    public:
        MeshColliderComponent();
        MeshColliderComponent(MeshColliderComponent& meshCollider);
        MeshColliderComponent(MeshColliderComponent&& meshCollider);
        ~MeshColliderComponent();
    public:
        virtual void OnInspector() override;
        virtual Component* Copy(entt::entity& handle, entt::registry& registry) override;
    public:
        void CreateMeshCollider(MeshData& data, TransformComponent& component);
        void Update(TransformComponent& component);
        void Destroy();
    private:
        reactphysics3d::ConcaveMeshShape* concaveMeshShape;
        reactphysics3d::TriangleMesh* triangleMesh;
        reactphysics3d::Collider* collider;
        Vector3 scale;

        VWOLF_COMPONENT_INSPECTOR_DEFINE(MeshColliderComponent);
    };
}
