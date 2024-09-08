//
//  MeshColliderComponent.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 7/2/24.
//

#include "vwpch.h"
#include "MeshColliderComponent.h"
#include "TransformComponent.h"
#include "VWolf/Core/Math/VMath.h"

#include "VWolf/Core/Physics/Physics.h"
#include "GameObject.h"

namespace VWolf {
    MeshColliderComponent::MeshColliderComponent(): Component("MeshCollider") {}

    MeshColliderComponent::MeshColliderComponent(MeshColliderComponent& meshCollider): Component("MeshCollider") {}

    MeshColliderComponent::MeshColliderComponent(MeshColliderComponent&& meshCollider): Component("MeshCollider") {}

    MeshColliderComponent::~MeshColliderComponent() {}

    void MeshColliderComponent::OnInspector() {
        MeshColliderComponent::componentInspector->OnInspector(this);
    }

    Component* MeshColliderComponent::Copy(entt::entity& handle, entt::registry& registry) {
        MeshColliderComponent& component = registry.emplace<MeshColliderComponent>(handle, *this);
        return &component;
    }

    void MeshColliderComponent::CreateMeshCollider(MeshData& data, TransformComponent& component) {

        std::vector<reactphysics3d::Message> messages;

        std::vector<float> positions;
        std::vector<float> normals;
        uint32_t nbVertices = (uint32_t)data.vertices.size();
        uint32_t nbTriangles = ((uint32_t)data.indices.size()) / 3;
        for(Vertex vertex: data.vertices) {
            positions.push_back(vertex.position.GetX());
            positions.push_back(vertex.position.GetY());
            positions.push_back(vertex.position.GetZ());

            normals.push_back(vertex.normal.GetX());
            normals.push_back(vertex.normal.GetY());
            normals.push_back(vertex.normal.GetZ());
        }
        reactphysics3d::TriangleVertexArray array(nbVertices, positions.data(), 3 * sizeof(float),
                                                  normals.data(), 3 * sizeof(float),
                                                  nbTriangles, data.indices.data(), 3 * sizeof(uint32_t),
                                                  reactphysics3d::TriangleVertexArray::VertexDataType::VERTEX_FLOAT_TYPE,
                                                  reactphysics3d::TriangleVertexArray::NormalDataType::NORMAL_FLOAT_TYPE,
                                                  reactphysics3d::TriangleVertexArray::IndexDataType::INDEX_INTEGER_TYPE);

        triangleMesh = Physics::GetCommon().createTriangleMesh(array, messages);
        
        if (triangleMesh == nullptr) return;

        scale = component.GetLocalScale();

        concaveMeshShape = Physics::GetCommon()
            .createConcaveMeshShape(triangleMesh, { scale.GetX(), scale.GetY(), scale.GetZ() });

        reactphysics3d::Transform transform = reactphysics3d::Transform::identity();
        reactphysics3d::RigidBody* rigidBody = GetGameObject()->GetRigidBody();
        if (rigidBody != nullptr) {
            collider = rigidBody->addCollider(concaveMeshShape, transform);
        }
    }

    void MeshColliderComponent::Update(TransformComponent& component) {
        if (concaveMeshShape != nullptr && component.GetLocalScale() != scale) {
            scale = component.GetLocalScale();
            concaveMeshShape->setScale({ scale.GetX(), scale.GetY(), scale.GetZ() });
        }
    }

    void MeshColliderComponent::Destroy() {
        if (collider != nullptr) {
            GetGameObject()->GetRigidBody()->removeCollider(collider);
        }
        if (concaveMeshShape != nullptr) {            
            Physics::GetCommon().destroyConcaveMeshShape(concaveMeshShape);
        }
        if (triangleMesh != nullptr)
            Physics::GetCommon().destroyTriangleMesh(triangleMesh);
    }

    VWOLF_COMPONENT_INSPECTOR_IMPLEMENTATION(MeshColliderComponent);
}
