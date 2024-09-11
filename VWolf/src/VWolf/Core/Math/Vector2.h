//
//  Vector2.h
//  VWolf
//
//  Created by Enrique Ricalde on 8/20/24.
//

#pragma once

#include "VWolf/Core/Base.h"

#include <glm/glm.hpp>

namespace VWolf {
    struct Vector3;
    struct Vector4;

    struct Vector2 {
    public:
        Vector2();
        Vector2(float, float);
        Vector2(Vector2 &);
        Vector2(const Vector2 &);
        Vector2(Vector2 &&);
        ~Vector2();
    public:
        Vector2& operator=(const Vector2& other);
//        Vector2& operator=(Vector2&& other);
    public:
        static const Vector2 Up;
        static const Vector2 Down;
        static const Vector2 Left;
        static const Vector2 Right;
        static const Vector2 Zero;
        static const Vector2 One;
        static const Vector2 NegativeInfinity;
        static const Vector2 PositiveInfinity;
    public:
        bool operator==(const Vector2& rhs);
        bool operator!=(const Vector2& rhs);
        Vector2 operator+(const Vector2& rhs);
        Vector2 operator-(const Vector2& rhs);
        Vector2 operator*(float rhs);
        Vector2 operator/(float rhs);
        float operator[](int index);
        operator Vector3();
        operator Vector4();
    public:
        Vector2 Normalized() const;
        void Normalize();
        float Magnitude() const;
        float SqrMagnitude() const;
    public:
        static float Angle(Vector2 from, Vector2 to);
        static Vector2 ClampMagnitude(Vector2 vector, float maxLength);
        static float Distance(Vector2 a, Vector2 b);
        static float Dot(Vector2 lhs, Vector2 rhs);
        static Vector2 Lerp(Vector2 a, Vector2 b, float t);
        static Vector2 LerpUnclamped(Vector2 a, Vector2 b, float t);
        static Vector2 Max(Vector2 lhs, Vector2 rhs);
        static Vector2 Min(Vector2 lhs, Vector2 rhs);
        static Vector2 MoveTowards(Vector2 current, Vector2 target, float maxDistanceDelta);
        static Vector2 Perpendicular(Vector2 inDirection);
        static Vector2 Reflect(Vector2 inDirection, Vector2 inNormal);
        static Vector2 Scale(Vector2 a, Vector2 b);
        static float SignedAngle(Vector2 from, Vector2 to);
        // SmoothDamp
    public:
        const float GetX() const;
        float& GetX();
        const float GetY() const;
        float& GetY();
        void SetX(float value);
        void SetY(float value);
    private:
        glm::vec2 _vector2;
#if defined(DEBUG)
    public:
        inline glm::vec2 GetInternalVector() const { return this->_vector2; }
#endif

        friend std::ostream& operator<<(std::ostream& os, const Vector2& v);
        friend bool operator==(const Vector2& lhs, const Vector2& rhs);
        friend bool operator!=(const Vector2& lhs, const Vector2& rhs);
        friend Vector2 operator+(const Vector2& lhs, const Vector2& rhs);
        friend Vector2 operator-(const Vector2& lhs, const Vector2& rhs);
        friend Vector2 operator*(const Vector2& lhs, float rhs);
        friend Vector2 operator/(const Vector2& lhs, float rhs);
        friend Vector2 operator*(float lhs, const Vector2& rhs);
        friend Vector2 operator/(float lhs, const Vector2& rhs);
    };
}
