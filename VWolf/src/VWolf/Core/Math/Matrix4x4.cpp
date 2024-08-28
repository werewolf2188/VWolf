//
//  Matrix4x4.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 8/27/24.
//

#include "vwpch.h"
#include "Matrix4x4.h"
#include "Vector4.h"
#include "Vector3.h"
#include "Quaternion.h"

#include <glm/gtx/matrix_decompose.hpp>

namespace VWolf {
    // MARK: Constants
    const Matrix4x4 Matrix4x4::Identity = Matrix4x4(1.0f);

    const Matrix4x4 Matrix4x4::Zero = Matrix4x4();

    // MARK: Constructors
    Matrix4x4::Matrix4x4(): _matrix4x4(glm::mat4x4(0)) { }

    Matrix4x4::Matrix4x4(float initializer): _matrix4x4(glm::mat4x4(initializer)) { }

    Matrix4x4::Matrix4x4(float c1r1, float c1r2, float c1r3, float c1r4,
                         float c2r1, float c2r2, float c2r3, float c2r4,
                         float c3r1, float c3r2, float c3r3, float c3r4,
                         float c4r1, float c4r2, float c4r3, float c4r4):
    _matrix4x4(glm::mat4x4(c1r1, c1r2, c1r3, c1r4,
                           c2r1, c2r2, c2r3, c2r4,
                           c3r1, c3r2, c3r3, c3r4,
                           c4r1, c4r2, c4r3, c4r4)) {}
    Matrix4x4::Matrix4x4(Vector4 c1, Vector4 c2, Vector4 c3, Vector4 c4):
    _matrix4x4(glm::mat4x4(c1.x, c1.y, c1.z, c1.w,
                           c2.x, c2.y, c2.z, c2.w,
                           c3.x, c3.y, c3.z, c3.w,
                           c4.x, c4.y, c4.z, c4.w)) {}

#if defined(VWOLF_CORE)
    Matrix4x4::Matrix4x4(glm::mat4x4 initializer): _matrix4x4(std::move(initializer)) { }
#endif

    Matrix4x4::Matrix4x4(const Matrix4x4& other): _matrix4x4(other._matrix4x4) { }

    Matrix4x4::Matrix4x4(Matrix4x4& other): _matrix4x4(other._matrix4x4) { }

    Matrix4x4::Matrix4x4(Matrix4x4&& other): _matrix4x4(std::move(other._matrix4x4)) {
        other._matrix4x4 = glm::mat4x4(0);
    }

    Matrix4x4::~Matrix4x4() {
        _matrix4x4 = glm::mat4x4(0);
    }

    // MARK: Assignment operators
    Matrix4x4& Matrix4x4::operator=(const Matrix4x4& other) {
        this->_matrix4x4 = other._matrix4x4;

        return *this;
    }

    // MARK: Operator overloading
    std::ostream& operator<<(std::ostream& os, const Matrix4x4& v) {
        os << "Matrix 4x4: \n";
        os << "[c1r1:" << v[0][0] << ", c1r2:" << v[0][1] << ", c1r3:" << v[0][2] << ", c1r4:" << v[0][3] << "] \n";
        os << "[c2r1:" << v[1][0] << ", c2r2:" << v[1][1] << ", c2r3:" << v[1][2] << ", c2r4:" << v[1][3] << "] \n";
        os << "[c3r1:" << v[2][0] << ", c3r2:" << v[2][1] << ", c3r3:" << v[2][2] << ", c3r4:" << v[2][3] << "] \n";
        os << "[c4r1:" << v[3][0] << ", c4r2:" << v[3][1] << ", c4r3:" << v[3][2] << ", c4r4:" << v[3][3] << "]";
        return os;
    }

    bool Matrix4x4::operator==(const Matrix4x4& rhs) {
        return _matrix4x4 == rhs._matrix4x4;
    }

    bool operator==(const Matrix4x4& lhs, const Matrix4x4& rhs) {
        return lhs.GetInternalMatrix() == rhs.GetInternalMatrix();
    }

    Matrix4x4 Matrix4x4::operator*(Matrix4x4 rhs) {
        glm::mat4x4 result = _matrix4x4 * rhs._matrix4x4;
        return Matrix4x4(result);
    }

    Vector4 Matrix4x4::operator[](int index) const {
        auto negative = std::numeric_limits<float>::max();
        if (index < 0 || index > 3) return Vector4(negative, negative, negative, negative);

        auto vec4 = _matrix4x4[index];
        return Vector4(vec4.x, vec4.y, vec4.z, vec4.w);
    }

    Matrix4x4 operator*(const Matrix4x4& lhs, Matrix4x4 rhs) {
        glm::mat4x4 result = lhs.GetInternalMatrix() * rhs.GetInternalMatrix();
        return Matrix4x4(result);
    }

    // MARK: Public Instance methods
    Vector4 Matrix4x4::GetColumn(int index) {
        return this->operator[](index);
    }

    Vector3 Matrix4x4::GetPosition() {
        glm::vec3 scale, translation, skew;
        glm::vec4 perspective;
        glm::quat orientation;
        glm::decompose(_matrix4x4, scale, orientation, translation, skew, perspective);

        return Vector3(translation.x, translation.y, translation.z);
    }

    Vector4 Matrix4x4::GetRow(int index) {
        float x = this->operator[](0)[index];
        float y = this->operator[](1)[index];
        float z = this->operator[](2)[index];
        float w = this->operator[](3)[index];
        return Vector4(x, y, z, w);
    }

    Vector3 Matrix4x4::MultiplyPoint(Vector3 point) {
        Vector4 point4 = static_cast<Vector4>(point);
        point4.w = 1.0f;
        glm::vec4 vec4 = point4.GetInternalVector();

        auto result = _matrix4x4 * vec4;
        return Vector3(result.x, result.y, result.z);
    }

    Vector3 Matrix4x4::MultiplyVector(Vector3 vector) {
        Vector4 vector4 = static_cast<Vector4>(vector);
        vector4.w = 0;
        glm::vec4 vec4 = vector4.GetInternalVector();

        auto result = _matrix4x4 * vec4;
        return Vector3(result.x, result.y, result.z);
    }

    void Matrix4x4::SetColumn(int index, Vector4 column) {
        _matrix4x4[index][0] = column.x;
        _matrix4x4[index][1] = column.y;
        _matrix4x4[index][2] = column.z;
        _matrix4x4[index][3] = column.w;
    }

    void Matrix4x4::SetRow(int index, Vector4 row) {
        _matrix4x4[0][index] = row.x;
        _matrix4x4[1][index] = row.y;
        _matrix4x4[2][index] = row.z;
        _matrix4x4[3][index] = row.w;
    }

    void Matrix4x4::SetTRS(Vector3 pos, Quaternion q, Vector3 s) {
        Vector3 rotation = q.EulerAngles();
        _matrix4x4 = glm::translate(_matrix4x4, pos.GetInternalVector());
        _matrix4x4 = glm::rotate(_matrix4x4, rotation.x, { 1.0f, 0.0f, 0.0f });
        _matrix4x4 = glm::rotate(_matrix4x4, rotation.y, { 0.0f, 1.0f, 0.0f });
        _matrix4x4 = glm::rotate(_matrix4x4, rotation.z, { 0.0f, 0.0f, 1.0f });
        _matrix4x4 = glm::scale(_matrix4x4, s.GetInternalVector());
    }

    // MARK: Static functions
    Matrix4x4 Matrix4x4::Frustum(float left, float right, float bottom, float top, float zNear, float zFar) {
        glm::mat4x4 mat = glm::frustum(left, right, bottom, top, zNear, zFar);
        return Matrix4x4(mat);
    }

    Matrix4x4 Matrix4x4::LookAt(Vector3 from, Vector3 to, Vector3 up) {
        glm::mat4x4 mat = glm::lookAt(from.GetInternalVector(), to.GetInternalVector(), up.GetInternalVector());

        return Matrix4x4(mat);
    }

    Matrix4x4 Matrix4x4::Ortho(float left, float right, float bottom, float top, float zNear, float zFar) {
        glm::mat4x4 mat = glm::ortho(left, right, bottom, top, zNear, zFar);
        return Matrix4x4(mat);
    }

    Matrix4x4 Matrix4x4::Perspective(float fov, float aspect, float zNear, float zFar) {
        glm::mat4x4 mat = glm::perspective(glm::radians(fov), aspect, zNear, zFar);
        return Matrix4x4(mat);
    }

    Matrix4x4 Matrix4x4::Rotate(Quaternion q) {
        Vector3 rotation = q.EulerAngles();
        glm::mat4x4 mat(1.0f);
        mat = glm::rotate(mat, rotation.x, { 1.0f, 0.0f, 0.0f });
        mat = glm::rotate(mat, rotation.y, { 0.0f, 1.0f, 0.0f });
        mat = glm::rotate(mat, rotation.z, { 0.0f, 0.0f, 1.0f });

        return Matrix4x4(mat);
    }

    Matrix4x4 Matrix4x4::Scale(Vector3 vector) {
        glm::mat4x4 mat(1.0f);
        glm::vec3 vec3 = vector.GetInternalVector();
        mat = glm::scale(mat, vec3);

        return Matrix4x4(mat);
    }

    Matrix4x4 Matrix4x4::Translate(Vector3 vector) {
        glm::mat4x4 mat(1.0f);
        glm::vec3 vec3 = vector.GetInternalVector();
        mat = glm::translate(mat, vec3);

        return Matrix4x4(mat);
    }

    Matrix4x4 Matrix4x4::TRS(Vector3 pos, Quaternion q, Vector3 s) {
        Matrix4x4 mat = Matrix4x4::Identity;
        mat.SetTRS(pos, q, s);

        return mat;
    }
}