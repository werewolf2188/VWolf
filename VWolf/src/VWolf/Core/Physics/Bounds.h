//
//  Bounds.h
//  VWolf
//
//  Created by Enrique Moises on 6/23/26.
//

#pragma once

#include "VWolf/Core/Math/Vector3.h"

namespace reactphysics3d {
    class AABB;
}

namespace VWolf {
    class Bounds {
    public:
        Bounds();
        Bounds(const std::vector<Vector3>& vector);
        Bounds(const Vector3& minX, const Vector3& max);
    public:
        Vector3 GetCenter();
        Vector3 GetExtents();
        Vector3 GetMin();
        Vector3 GetMax();
        Vector3 GetSize();
    public:
        bool Contains(Vector3 point);
        void Encapsulate(Vector3 point);
        void Encapsulate(const Bounds& bounds);
        void Expand(float amount);
        void Expand(Vector3 amount);
        bool Intersects(const Bounds& bounds);
        void SetMinMax(Vector3 min, Vector3 max);
        std::string ToString();
    private:
        void Initialize(const Vector3& minX, const Vector3& max);
    private:
        Ref<reactphysics3d::AABB> aabb;
    };
}
