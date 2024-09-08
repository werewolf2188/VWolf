//
//  IVector2.hpp
//  VWolf
//
//  Created by Enrique Ricalde on 8/21/24.
//

#pragma once

#include "VWolf/Core/Base.h"

#include <glm/glm.hpp>

namespace VWolf {
    struct Vector2;

    struct IVector2 {
    public:
        IVector2();
        IVector2(int32_t, int32_t);
        IVector2(IVector2 &);
        IVector2(const IVector2 &);
        IVector2(IVector2 &&);
        ~IVector2();
    public:
        IVector2& operator=(const IVector2& other);
//        Vector2& operator=(Vector2&& other);
    public:
        static const IVector2 Up;
        static const IVector2 Down;
        static const IVector2 Left;
        static const IVector2 Right;
        static const IVector2 Zero;
        static const IVector2 One;
    public:
        bool operator==(const IVector2& rhs);
        bool operator!=(const IVector2& rhs);
        IVector2 operator+(const IVector2& rhs);
        IVector2 operator-(const IVector2& rhs);
        IVector2 operator*(int32_t rhs);
        IVector2 operator/(int32_t rhs);
        int32_t operator[](int index);
        operator Vector2();
    public:
        float Magnitude() const;
        int32_t SqrMagnitude() const;
    public:
        static IVector2 CeilToInt(Vector2 vector);
        static float Distance(IVector2 a, IVector2 b);
        static IVector2 FloorToInt(Vector2 vector);
        static IVector2 Max(IVector2 lhs, IVector2 rhs);
        static IVector2 Min(IVector2 lhs, IVector2 rhs);
        static IVector2 RoundToInt(Vector2 vector);
        static IVector2 Scale(IVector2 a, IVector2 b);
    public:
        const int32_t GetX() const;
        int32_t& GetX();
        const int32_t GetY() const;
        int32_t& GetY();
        void SetX(int32_t value);
        void SetY(int32_t value);
    private:
        glm::ivec2 _vector2;
#if defined(DEBUG)
    public:
        inline glm::ivec2 GetInternalVector() const { return this->_vector2; }
#endif
        friend std::ostream& operator<<(std::ostream& os, const IVector2& v);
        friend bool operator==(const IVector2& lhs, const IVector2& rhs);
        friend bool operator!=(const IVector2& lhs, const IVector2& rhs);
        friend IVector2 operator+(const IVector2& lhs, const IVector2& rhs);
        friend IVector2 operator-(const IVector2& lhs, const IVector2& rhs);
        friend IVector2 operator*(const IVector2& lhs, int32_t rhs);
        friend IVector2 operator*(int32_t lhs, const IVector2& rhs);
        friend IVector2 operator/(const IVector2& lhs, int32_t rhs);
        friend IVector2 operator/(int32_t lhs, const IVector2& rhs);
    };
}
