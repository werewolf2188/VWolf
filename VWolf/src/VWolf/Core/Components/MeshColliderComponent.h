//
//  MeshColliderComponent.hpp
//  VWolf
//
//  Created by Enrique Ricalde on 7/2/24.
//

#pragma once

#include "BaseComponent.h"
#include "VWolf/Core/Render/Mesh.h"

#include "VWolf/Core/Utils/GenericSerialization.h"

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
        MeshColliderComponent(const MeshColliderComponent& meshCollider);
        MeshColliderComponent(MeshColliderComponent&& meshCollider);
        ~MeshColliderComponent();
    public:
        virtual Ref<Component> Copy(entt::entity& handle, entt::registry& registry) override;
    public:
        void CreateMeshCollider(Ref<Mesh> data, TransformComponent& component);
        void Update(TransformComponent& component);
        void Destroy();
    private:
        Ref<reactphysics3d::ConcaveMeshShape> concaveMeshShape;
        Ref<reactphysics3d::TriangleMesh> triangleMesh;
        Ref<reactphysics3d::Collider> collider;
        Vector3 scale;

        BOOST_DESCRIBE_CLASS(MeshColliderComponent, (Component), (), (id), ())
        VWOLF_SERIALIZATION_FRIENDS(MeshColliderComponent)
    };
}

namespace YAML {
    VWOLF_CREATE_CONVERT_GENERIC_CLASS_DECODER(VWolf::MeshColliderComponent)
}
