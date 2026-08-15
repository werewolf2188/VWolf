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
    MeshColliderComponent::MeshColliderComponent():
    Component(ClassNameCleaner::Current().GetClassName<MeshColliderComponent>()) {}

    MeshColliderComponent::MeshColliderComponent(const MeshColliderComponent& meshCollider):
    Component(ClassNameCleaner::Current().GetClassName<MeshColliderComponent>(), meshCollider.id) {}

    MeshColliderComponent::MeshColliderComponent(MeshColliderComponent&& meshCollider):
    Component(ClassNameCleaner::Current().GetClassName<MeshColliderComponent>(), meshCollider.id) {}

    MeshColliderComponent::~MeshColliderComponent() {}

    Ref<Component> MeshColliderComponent::Copy(entt::entity& handle, entt::registry& registry) {
        Ref<MeshColliderComponent> component = CopyComponent<MeshColliderComponent>(handle, registry);
        return component;
    }

    void MeshColliderComponent::CreateMeshCollider(Ref<Mesh> data, TransformComponent& component) {

        std::vector<reactphysics3d::Message> messages;

        std::vector<float> positions;
        std::vector<float> normals;
        uint32_t nbVertices = (uint32_t)data->GetVertices().size();
        uint32_t nbTriangles = ((uint32_t)data->GetTriangles().size()) / 3;
        for(uint32_t index = 0; index < data->GetVertices().size(); index++) {
            positions.push_back(data->GetVertices()[index].GetX());
            positions.push_back(data->GetVertices()[index].GetY());
            positions.push_back(data->GetVertices()[index].GetZ());

            normals.push_back(data->GetNormals()[index].GetX());
            normals.push_back(data->GetNormals()[index].GetY());
            normals.push_back(data->GetNormals()[index].GetZ());
        }
        reactphysics3d::TriangleVertexArray array(nbVertices, positions.data(), 3 * sizeof(float),
                                                  normals.data(), 3 * sizeof(float),
                                                  nbTriangles, data->GetTriangles().data(), 3 * sizeof(uint32_t),
                                                  reactphysics3d::TriangleVertexArray::VertexDataType::VERTEX_FLOAT_TYPE,
                                                  reactphysics3d::TriangleVertexArray::NormalDataType::NORMAL_FLOAT_TYPE,
                                                  reactphysics3d::TriangleVertexArray::IndexDataType::INDEX_INTEGER_TYPE);

        triangleMesh = Ref<reactphysics3d::TriangleMesh>(Physics::GetCommon().createTriangleMesh(array, messages), [this](reactphysics3d::TriangleMesh * triangleMesh) {
            if (GetGameObject() != nullptr)
                Physics::GetCommon().destroyTriangleMesh(triangleMesh);
        });
        
        if (triangleMesh == nullptr) return;

        scale = component.GetLocalScale();

        concaveMeshShape = Ref<reactphysics3d::ConcaveMeshShape>(Physics::GetCommon()
                                 .createConcaveMeshShape(triangleMesh.get(), { scale.GetX(), scale.GetY(), scale.GetZ() }), [this](reactphysics3d::ConcaveMeshShape * concaveMeshShape) {
                                    if (GetGameObject() != nullptr)
                                         Physics::GetCommon().destroyConcaveMeshShape(concaveMeshShape);
                                 });

        reactphysics3d::Transform transform = reactphysics3d::Transform::identity();
        reactphysics3d::RigidBody* rigidBody = GetGameObject()->GetRigidBody();
        if (rigidBody != nullptr) {
            collider = Ref<reactphysics3d::Collider>(rigidBody->addCollider(concaveMeshShape.get(), transform), [this](reactphysics3d::Collider * collider) {
                if (GetGameObject() != nullptr)
                    GetGameObject()->GetRigidBody()->removeCollider(collider);
            });
        }
    }

    void MeshColliderComponent::Update(TransformComponent& component) {
        if (concaveMeshShape != nullptr && component.GetLocalScale() != scale) {
            scale = component.GetLocalScale();
            concaveMeshShape->setScale({ scale.GetX(), scale.GetY(), scale.GetZ() });
        }
    }

    void MeshColliderComponent::Destroy() {
        collider = nullptr;
        concaveMeshShape = nullptr;
        triangleMesh = nullptr;
    }

    VWOLF_CREATE_CONVERT_GENERIC_CLASS_ENCODER_WITH_NAME(MeshColliderComponent, "MeshColliderComponent")
}

/*
 Convex hull
 #include <iostream>
 #include <vector>
 #include <algorithm>

 // Define a structure to represent a 2D Point
 struct Point {
     double x, y;

     // Operator for sorting points lexicographically
     bool operator<(const Point& other) const {
         if (x != other.x) return x < other.x;
         return y < other.y;
     }
 };

 // Returns the cross product of vectors AB and AC.
 // Positive value indicates a counter-clockwise turn.
 // Negative value indicates a clockwise turn.
 // Zero indicates that the points are collinear.
 double cross_product(const Point& A, const Point& B, const Point& C) {
     return (B.x - A.x) * (C.y - A.y) - (B.y - A.y) * (C.x - A.x);
 }

 // Computes the convex hull of a list of points
 std::vector<Point> convex_hull(std::vector<Point>& points) {
     size_t n = points.size();
     if (n <= 3) return points; // A hull is trivial for 3 or fewer points

     // Step 1: Sort points lexicographically by x, then by y
     std::sort(points.begin(), points.end());

     std::vector<Point> hull;

     // Step 2: Build the Lower Hull
     for (size_t i = 0; i < n; ++i) {
         while (hull.size() >= 2 && cross_product(hull[hull.size() - 2], hull.back(), points[i]) <= 0) {
             hull.pop_back(); // Pop if the turn is clockwise or collinear
         }
         hull.push_back(points[i]);
     }

     // Step 3: Build the Upper Hull
     size_t lower_hull_size = hull.size();
     for (int i = static_cast<int>(n) - 2; i >= 0; --i) {
         while (hull.size() > lower_hull_size && cross_product(hull[hull.size() - 2], hull.back(), points[i]) <= 0) {
             hull.pop_back();
         }
         hull.push_back(points[i]);
     }

     // Remove the last point because it is the same as the first point
     hull.pop_back();

     return hull;
 }
 */
