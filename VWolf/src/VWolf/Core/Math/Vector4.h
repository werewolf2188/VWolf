//
//  Vector4.hpp
//  VWolf
//
//  Created by Enrique Ricalde on 8/22/24.
//

#pragma once
#include "VWolf/Core/Base.h"

#include <glm/glm.hpp>

namespace VWolf {
    struct Vector2;
    struct Vector3;

    struct Vector4 {
    public:
        Vector4();
        Vector4(float, float, float, float);
        Vector4(Vector4 &);
        Vector4(Vector4 &&);
        ~Vector4();
    public:
        Vector4& operator=(const Vector4& other);
//        Vector2& operator=(Vector2&& other);
    public:
        static const Vector4 Zero;
        static const Vector4 One;
        static const Vector4 NegativeInfinity;
        static const Vector4 PositiveInfinity;
    public:
        bool operator==(const Vector4& rhs);
        bool operator!=(const Vector4& rhs);
        Vector4 operator+(const Vector4& rhs);
        Vector4 operator-(const Vector4& rhs);
        Vector4 operator*(float rhs);
        Vector4 operator/(float rhs);
        float operator[](int index);
        operator Vector2();
        operator Vector3();
    public:
        Vector4 Normalized() const;
        void Normalize();
        float Magnitude() const;
        float SqrMagnitude() const;
    public:
        static float Distance(Vector4 a, Vector4 b);
        static float Dot(Vector4 lhs, Vector4 rhs);
        static Vector4 Lerp(Vector4 a, Vector4 b, float t);
        static Vector4 LerpUnclamped(Vector4 a, Vector4 b, float t);
        static Vector4 Max(Vector4 lhs, Vector4 rhs);
        static Vector4 Min(Vector4 lhs, Vector4 rhs);
        static Vector4 MoveTowards(Vector4 current, Vector4 target, float maxDistanceDelta);
        static Vector4 ProjectOnPlane(Vector4 vector, Vector4 planeNormal);
        static Vector4 Reflect(Vector4 inDirection, Vector4 inNormal);
        static Vector4 Scale(Vector4 a, Vector4 b);
        // SmoothDamp
    public:
        float& x;
        float& y;
        float& z;
        float& w;
    private:
        glm::vec4 _vector4;
#if defined(DEBUG)
    public:
        inline glm::vec4& GetInternalVector() { return this->_vector4; }
#endif
    };

    std::ostream& operator<<(std::ostream& os, const Vector4& v);
    bool operator==(const Vector4& lhs, const Vector4& rhs);
    bool operator!=(const Vector4& lhs, const Vector4& rhs);
    Vector4 operator+(const Vector4& lhs, const Vector4& rhs);
    Vector4 operator-(const Vector4& lhs, const Vector4& rhs);
    Vector4 operator*(const Vector4& lhs, float rhs);
    Vector4 operator/(const Vector4& lhs, float rhs);
}