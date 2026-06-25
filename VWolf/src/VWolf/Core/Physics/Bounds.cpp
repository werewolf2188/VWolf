//
//  Bounds.cpp
//  VWolf
//
//  Created by Enrique Moises on 6/23/26.
//

#include "vwpch.h"
#include "Bounds.h"

#include "reactphysics3d/reactphysics3d.h"

namespace VWolf {
    Bounds::Bounds() {
        aabb = CreateRef<reactphysics3d::AABB>();
    }

    Bounds::Bounds(const std::vector<Vector3>& vertices) {
        std::vector<float> x(vertices.size()), y(vertices.size()), z(vertices.size());
        
        std::transform(vertices.begin(), vertices.end(), x.begin(), [](Vector3 vertex) { return vertex.GetX(); });
        std::transform(vertices.begin(), vertices.end(), y.begin(), [](Vector3 vertex) { return vertex.GetY(); });
        std::transform(vertices.begin(), vertices.end(), z.begin(), [](Vector3 vertex) { return vertex.GetZ(); });
        
        float minX = *std::min_element(x.begin(), x.end());
        float maxX = *std::max_element(x.begin(), x.end());
        
        float minY = *std::min_element(y.begin(), y.end());
        float maxY = *std::max_element(y.begin(), y.end());
        
        float minZ = *std::min_element(z.begin(), z.end());
        float maxZ = *std::max_element(z.begin(), z.end());
        
        Vector3 min1(minX, minY, minZ);
        Vector3 max1(maxX, maxY, maxZ);
        
        Initialize(min1, max1);
    }

    Bounds::Bounds(const Vector3& minX, const Vector3& max) {
        Initialize(minX, max);
    }

    void Bounds::Initialize(const Vector3& min, const Vector3& max) {
        reactphysics3d::Vector3 min1(min.GetX(), min.GetY(), min.GetZ());
        reactphysics3d::Vector3 max1(max.GetX(), max.GetY(), max.GetZ());
        aabb = CreateRef<reactphysics3d::AABB>(min1, max1);
    }

    Vector3 Bounds::GetCenter() {
        reactphysics3d::Vector3 vector = aabb->getCenter();
        return Vector3(vector.x, vector.y, vector.z);
    }

    Vector3 Bounds::GetExtents() {
        reactphysics3d::Vector3 vector = aabb->getExtent();
        return Vector3(vector.x, vector.y, vector.z);
    }

    Vector3 Bounds::GetMin() {
        reactphysics3d::Vector3 vector = aabb->getMin();
        return Vector3(vector.x, vector.y, vector.z);
    }

    Vector3 Bounds::GetMax() {
        reactphysics3d::Vector3 vector = aabb->getMax();
        return Vector3(vector.x, vector.y, vector.z);
    }

    Vector3 Bounds::GetSize() {
        reactphysics3d::Vector3 vector = (aabb->getExtent() * 2);
        return Vector3(vector.x, vector.y, vector.z);
    }

    bool Bounds::Contains(Vector3 point) {
        reactphysics3d::Vector3 vector(point.GetX(), point.GetY(), point.GetZ());
        return aabb->contains(vector);
    }

    void Bounds::Encapsulate(Vector3 point) {
        reactphysics3d::Vector3 vector(point.GetX(), point.GetY(), point.GetZ());
        aabb->inflateWithPoint(vector);
    }

    void Bounds::Encapsulate(const Bounds& bounds) {
        const reactphysics3d::AABB& temp = *bounds.aabb;
        aabb->mergeWithAABB(temp);
    }

    void Bounds::Expand(float amount) {
        aabb->inflate(amount * 2, amount * 2, amount * 2);
    }

    void Bounds::Expand(Vector3 amount) {
        aabb->inflate(amount.GetX() * 2, amount.GetY() * 2, amount.GetZ() * 2);
    }

    bool Bounds::Intersects(const Bounds& bounds) {
        const reactphysics3d::AABB& temp = *bounds.aabb;
        return aabb->testCollision(temp);
    }

    void Bounds::SetMinMax(Vector3 min, Vector3 max) {
        reactphysics3d::Vector3 min1(min.GetX(), min.GetY(), min.GetZ());
        reactphysics3d::Vector3 max1(max.GetX(), max.GetY(), max.GetZ());
        aabb->setMax(max1);
        aabb->setMin(min1);
    }

    std::string Bounds::ToString() {
        std::stringstream ss;
        ss << "Bounds (center: " << GetCenter() << ", extents: " << GetExtents() << ")";        
        return ss.str();
    }
}
