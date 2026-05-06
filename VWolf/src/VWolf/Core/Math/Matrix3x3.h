//
//  Matrix3x3.hpp
//  VWolf
//
//  Created by Enrique Moises on 5/5/26.
//

#pragma once

#include <glm/glm.hpp>
#include "VWolf/Core/Math/Serialization/glm.h"
#include "VWolf/Core/Math/Serialization/VMath.h"

namespace VWolf {
    struct Vector3;
    struct Matrix4x4;

    struct Matrix3x3 {
    public:
        Matrix3x3();
        Matrix3x3(float initializer);
        Matrix3x3(float c1r1, float c1r2, float c1r3,
                  float c2r1, float c2r2, float c2r3,
                  float c3r1, float c3r2, float c3r3);
        Matrix3x3(Vector3 c1, Vector3 c2, Vector3 c3);
        Matrix3x3(const Matrix3x3&);
        Matrix3x3(Matrix3x3&);
        Matrix3x3(Matrix3x3&&);
        ~Matrix3x3();
    private:
        Matrix3x3(glm::mat3x3);
    public:
        static const Matrix3x3 Identity;
        static const Matrix3x3 Zero;
    public:
        Matrix3x3& operator=(const Matrix3x3& other);
    public:
        bool operator==(const Matrix3x3& rhs);
        Matrix3x3 operator*(Matrix3x3 rhs);
        Vector3 operator[](int index) const;
        operator Matrix4x4() const;
    public:
        float GetDeterminant() const;
        Matrix3x3 GetInverse() const;
        bool IsIdentity() const;
        Matrix3x3 GetTranspose() const;
    public:
        Vector3 GetColumn(int index);
        Vector3 GetRow(int index);
        void SetColumn(int index, Vector3 column);
        void SetRow(int index, Vector3 row);
        float* Unsafe_AddressOf();
    private:
        glm::mat3x3 _matrix3x3;
    #if defined(DEBUG)
    public:
        inline glm::mat3x3 GetInternalMatrix() const { return this->_matrix3x3; }
    #endif
        friend std::ostream& operator<<(std::ostream& os, const Matrix3x3& q);
        friend bool operator==(const Matrix3x3& lhs, const Matrix3x3& rhs);
        friend Matrix3x3 operator*(const Matrix3x3& lhs, Matrix3x3 rhs);

        friend struct Vector3;
        
        VWOLF_VMATH_SERIALIZATION_FRIENDS(Matrix3x3)
    };
}

namespace YAML {
    VWOLF_VMATH_SERIALIZATION_MATRIX_DECODER(VWolf::Matrix3x3, glm::mat3x3, 3, 3)
}
