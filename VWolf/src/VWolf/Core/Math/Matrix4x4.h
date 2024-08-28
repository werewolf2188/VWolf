//
//  Matrix4x4.hpp
//  VWolf
//
//  Created by Enrique Ricalde on 8/27/24.
//

#pragma once

#ifndef GLM_ENABLE_EXPERIMENTAL
#define GLM_ENABLE_EXPERIMENTAL
#endif

#include <glm/glm.hpp>

namespace VWolf {
    struct Vector3;
    struct Vector4;
    struct Quaternion;

    struct Matrix4x4 {
    public:
        Matrix4x4();
        Matrix4x4(float initializer);
        Matrix4x4(float c1r1, float c1r2, float c1r3, float c1r4,
                  float c2r1, float c2r2, float c2r3, float c2r4,
                  float c3r1, float c3r2, float c3r3, float c3r4,
                  float c4r1, float c4r2, float c4r3, float c4r4);
#if defined(VWOLF_CORE)
        Matrix4x4(glm::mat4x4);
#endif
        Matrix4x4(Vector4 c1, Vector4 c2, Vector4 c3, Vector4 c4);
        Matrix4x4(const Matrix4x4&);
        Matrix4x4(Matrix4x4&);
        Matrix4x4(Matrix4x4&&);
        ~Matrix4x4();
    public:
        static const Matrix4x4 Identity;
        static const Matrix4x4 Zero;
    public:
        Matrix4x4& operator=(const Matrix4x4& other);
    public:
        bool operator==(const Matrix4x4& rhs);
        Matrix4x4 operator*(Matrix4x4 rhs);
        Vector4 operator[](int index) const;
    public:
        Vector4 GetColumn(int index);
        Vector3 GetPosition();
        Vector4 GetRow(int index);
        Vector3 MultiplyPoint(Vector3 point);
        Vector3 MultiplyVector(Vector3 vector);
        void SetColumn(int index, Vector4 column);
        void SetRow(int index, Vector4 row);
        void SetTRS(Vector3 pos, Quaternion q, Vector3 s);
    public:
        static Matrix4x4 Frustum(float left, float right, float bottom, float top, float zNear, float zFar);
        static Matrix4x4 LookAt(Vector3 from, Vector3 to, Vector3 up);
        static Matrix4x4 Ortho(float left, float right, float bottom, float top, float zNear, float zFar);
        static Matrix4x4 Perspective(float fov, float aspect, float zNear, float zFar);
        static Matrix4x4 Rotate(Quaternion q);
        static Matrix4x4 Scale(Vector3 vector);
        static Matrix4x4 Translate(Vector3 vector);
        static Matrix4x4 TRS(Vector3 pos, Quaternion q, Vector3 s);
    private:
        glm::mat4x4 _matrix4x4;
#if defined(DEBUG) || defined(VWOLF_CORE)
    public:
        inline glm::mat4x4 GetInternalMatrix() const { return this->_matrix4x4; }
#endif
    };

    std::ostream& operator<<(std::ostream& os, const Matrix4x4& q);
    bool operator==(const Matrix4x4& lhs, const Matrix4x4& rhs);
    Matrix4x4 operator*(const Matrix4x4& lhs, Matrix4x4 rhs);
}

