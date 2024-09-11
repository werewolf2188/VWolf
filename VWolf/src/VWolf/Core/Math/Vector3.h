//
//  Vector3.hpp
//  VWolf
//
//  Created by Enrique Ricalde on 8/22/24.
//

#pragma once
#include "VWolf/Core/Base.h"

#include <glm/glm.hpp>

namespace VWolf {
    struct Vector2;
    struct Vector4;
    struct Matrix4x4;
    struct Quaternion;

    struct Vector3 {
    public:
        Vector3();
        Vector3(float, float, float);
        Vector3(Vector3 &);
        Vector3(const Vector3 &);
        Vector3(Vector3 &&);
        ~Vector3();
    private:
        Vector3(glm::vec3);
    public:
        Vector3& operator=(const Vector3& other);
//        Vector2& operator=(Vector2&& other);
    public:
        static const Vector3 Up;
        static const Vector3 Down;
        static const Vector3 Left;
        static const Vector3 Right;
        static const Vector3 Back;
        static const Vector3 Forward;
        static const Vector3 Zero;
        static const Vector3 One;
        static const Vector3 NegativeInfinity;
        static const Vector3 PositiveInfinity;
    public:
        bool operator==(const Vector3& rhs);
        bool operator!=(const Vector3& rhs);
        Vector3 operator+(const Vector3& rhs);
        Vector3& operator+=(const Vector3& rhs);
        Vector3 operator-(const Vector3& rhs);
        Vector3 operator-();
        Vector3 operator*(float rhs);
        Vector3 operator/(float rhs);
        float operator[](int index);
        operator Vector2();
        operator Vector4();
    public:
        Vector3 Normalized() const;
        void Normalize();
        float Magnitude() const;
        float SqrMagnitude() const;
        Vector3 Degrees() const;
        Matrix4x4 Orientate() const;
    public:
        static float Angle(Vector3 from, Vector3 to);
        static Vector3 ClampMagnitude(Vector3 vector, float maxLength);
        static Vector3 Cross(Vector3 a, Vector3 b);
        static float Distance(Vector3 a, Vector3 b);
        static float Dot(Vector3 lhs, Vector3 rhs);
        static Vector3 Lerp(Vector3 a, Vector3 b, float t);
        static Vector3 LerpUnclamped(Vector3 a, Vector3 b, float t);
        static Vector3 Max(Vector3 lhs, Vector3 rhs);
        static Vector3 Min(Vector3 lhs, Vector3 rhs);
        static Vector3 MoveTowards(Vector3 current, Vector3 target, float maxDistanceDelta);
        static Vector3 Perpendicular(Vector3 inDirection);
        static Vector3 Project(Vector3 vector, Vector3 onNormal);
        static Vector3 ProjectOnPlane(Vector3 vector, Vector3 planeNormal);
        static Vector3 Reflect(Vector3 inDirection, Vector3 inNormal);
        static Vector3 Scale(Vector3 a, Vector3 b);
        static Vector3 Slerp(Vector3 a, Vector3 b, float t);
        static Vector3 SlerpUnclamped(Vector3 a, Vector3 b, float t);
        static float SignedAngle(Vector3 from, Vector3 to);
        // SmoothDamp
    public:
        const float GetX() const;
        float& GetX();
        const float GetY() const;
        float& GetY();
        const float GetZ() const;
        float& GetZ();
        void SetX(float value);
        void SetY(float value);
        void SetZ(float value);
    private:
        glm::vec3 _vector3;
#if defined(DEBUG)
    public:
        inline glm::vec3 GetInternalVector() const { return this->_vector3; }
#endif
        friend std::ostream& operator<<(std::ostream& os, const Vector3& v);
        friend bool operator==(const Vector3& lhs, const Vector3& rhs);
        friend bool operator!=(const Vector3& lhs, const Vector3& rhs);
        friend Vector3 operator+(const Vector3& lhs, const Vector3& rhs);
        friend Vector3 operator-(const Vector3& lhs, const Vector3& rhs);
        friend Vector3 operator*(const Vector3& lhs, float rhs);
        friend Vector3 operator*(float lhs, const Vector3& rhs);
        friend Vector3 operator/(const Vector3& lhs, float rhs);
        friend Vector3 operator/(float lhs, const Vector3& rhs);

        friend struct Quaternion;
        friend struct Matrix4x4;
    };
}
