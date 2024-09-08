//
//  IVector3.hpp
//  VWolf
//
//  Created by Enrique Ricalde on 8/22/24.
//

#pragma once
#include "VWolf/Core/Base.h"

#include <glm/glm.hpp>

namespace VWolf {
    struct IVector2;
    struct Vector3;

    struct IVector3 {
    public:
        IVector3();
        IVector3(int32_t, int32_t, int32_t);
        IVector3(IVector3 &);
        IVector3(const IVector3 &);
        IVector3(IVector3 &&);
        ~IVector3();
    public:
        IVector3& operator=(const IVector3& other);
//        Vector2& operator=(Vector2&& other);
    public:
        static const IVector3 Up;
        static const IVector3 Down;
        static const IVector3 Left;
        static const IVector3 Right;
        static const IVector3 Back;
        static const IVector3 Forward;
        static const IVector3 Zero;
        static const IVector3 One;
    public:
        bool operator==(const IVector3& rhs);
        bool operator!=(const IVector3& rhs);
        IVector3 operator+(const IVector3& rhs);
        IVector3 operator-(const IVector3& rhs);
        IVector3 operator*(int32_t rhs);
        IVector3 operator/(int32_t rhs);
        int32_t operator[](int index);
        operator IVector2();
        operator Vector3();
    public:
        float Magnitude() const;
        float SqrMagnitude() const;
    public:
        static IVector3 CeilToInt(Vector3 vector);
        static float Distance(IVector3 a, IVector3 b);
        static IVector3 FloorToInt(Vector3 vector);
        static IVector3 Max(IVector3 lhs, IVector3 rhs);
        static IVector3 Min(IVector3 lhs, IVector3 rhs);
        static IVector3 RoundToInt(Vector3 vector);
        static IVector3 Scale(IVector3 a, IVector3 b);
    public:
        const int32_t GetX() const;
        int32_t& GetX();
        const int32_t GetY() const;
        int32_t& GetY();
        const int32_t GetZ() const;
        int32_t& GetZ();
        void SetX(int32_t value);
        void SetY(int32_t value);
        void SetZ(int32_t value);
    private:
        glm::ivec3 _vector3;
#if defined(DEBUG)
    public:
        inline glm::ivec3 GetInternalVector() const { return this->_vector3; }
#endif
        friend std::ostream& operator<<(std::ostream& os, const IVector3& v);
        friend bool operator==(const IVector3& lhs, const IVector3& rhs);
        friend bool operator!=(const IVector3& lhs, const IVector3& rhs);
        friend IVector3 operator+(const IVector3& lhs, const IVector3& rhs);
        friend IVector3 operator-(const IVector3& lhs, const IVector3& rhs);
        friend IVector3 operator*(const IVector3& lhs, int32_t rhs);
        friend IVector3 operator*(int32_t lhs, const IVector3& rhs);
        friend IVector3 operator/(const IVector3& lhs, int32_t rhs);
        friend IVector3 operator/(int32_t lhs, const IVector3& rhs);
    };
}
