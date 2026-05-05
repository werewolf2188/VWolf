//
//  Matrix3x3.cpp
//  VWolf
//
//  Created by Enrique Moises on 5/5/26.
//

#include "vwpch.h"
#include "Matrix3x3.h"
#include "Matrix4x4.h"
#include "Vector3.h"

namespace VWolf {
    // MARK: Constants
    const Matrix3x3 Matrix3x3::Identity = Matrix3x3(1.0f);

    const Matrix3x3 Matrix3x3::Zero = Matrix3x3();

    // MARK: Constructors
    Matrix3x3::Matrix3x3(): _matrix3x3(glm::mat3x3(0)) { }

    Matrix3x3::Matrix3x3(float initializer): _matrix3x3(glm::mat3x3(initializer)) { }

    Matrix3x3::Matrix3x3(float c1r1, float c1r2, float c1r3,
                         float c2r1, float c2r2, float c2r3,
                         float c3r1, float c3r2, float c3r3):
    _matrix3x3(glm::mat3x3(c1r1, c1r2, c1r3,
                           c2r1, c2r2, c2r3,
                           c3r1, c3r2, c3r3)) {}
    Matrix3x3::Matrix3x3(Vector3 c1, Vector3 c2, Vector3 c3):
    _matrix3x3(glm::mat3x3(c1.GetX(), c1.GetY(), c1.GetZ(),
                           c2.GetX(), c2.GetY(), c2.GetZ(),
                           c3.GetX(), c3.GetY(), c3.GetZ())) {}

    Matrix3x3::Matrix3x3(glm::mat3x3 initializer): _matrix3x3(std::move(initializer)) { }

    Matrix3x3::Matrix3x3(const Matrix3x3& other): _matrix3x3(other._matrix3x3) { }

    Matrix3x3::Matrix3x3(Matrix3x3& other): _matrix3x3(other._matrix3x3) { }

    Matrix3x3::Matrix3x3(Matrix3x3&& other): _matrix3x3(std::move(other._matrix3x3)) {
        other._matrix3x3 = glm::mat3x3(0);
    }

    Matrix3x3::~Matrix3x3() {
        _matrix3x3 = glm::mat3x3(0);
    }

    // MARK: Assignment operators
    Matrix3x3& Matrix3x3::operator=(const Matrix3x3& other) {
        this->_matrix3x3 = other._matrix3x3;

        return *this;
    }

    // MARK: Operator overloading
    std::ostream& operator<<(std::ostream& os, const Matrix3x3& v) {
        os << "Matrix 4x4: \n";
        os << "[c1r1:" << v[0][0] << ", c1r2:" << v[0][1] << ", c1r3:" << v[0][2] << "] \n";
        os << "[c2r1:" << v[1][0] << ", c2r2:" << v[1][1] << ", c2r3:" << v[1][2] << "] \n";
        os << "[c3r1:" << v[2][0] << ", c3r2:" << v[2][1] << ", c3r3:" << v[2][2] << "]";
        return os;
    }

    bool Matrix3x3::operator==(const Matrix3x3& rhs) {
        return _matrix3x3 == rhs._matrix3x3;
    }

    bool operator==(const Matrix3x3& lhs, const Matrix3x3& rhs) {
        return lhs._matrix3x3 == rhs._matrix3x3;
    }

    Matrix3x3 Matrix3x3::operator*(Matrix3x3 rhs) {
        glm::mat3x3 result = _matrix3x3 * rhs._matrix3x3;
        return Matrix3x3(result);
    }

    Matrix3x3::operator Matrix4x4() const {
        return Matrix4x4(
                         _matrix3x3[0][0], _matrix3x3[0][1], _matrix3x3[0][2], 0,
                         _matrix3x3[1][0], _matrix3x3[1][1], _matrix3x3[1][2], 0,
                         _matrix3x3[2][0], _matrix3x3[2][1], _matrix3x3[2][2], 0,
                         0, 0, 0, 0
                         );
    }

    Vector3 Matrix3x3::operator[](int index) const {
        auto negative = std::numeric_limits<float>::max();
        if (index < 0 || index > 2) return Vector3(negative, negative, negative);

        auto vec3 = _matrix3x3[index];
        return Vector3(vec3.x, vec3.y, vec3.z);
    }

    Matrix3x3 operator*(const Matrix3x3& lhs, Matrix3x3 rhs) {
        glm::mat3x3 result = lhs._matrix3x3 * rhs._matrix3x3;
        return Matrix3x3(result);
    }

    // MARK: Public Get methods
    float Matrix3x3::GetDeterminant() const {
        return glm::determinant(_matrix3x3);
    }

    Matrix3x3 Matrix3x3::GetInverse() const {
        glm::mat3x3 result = glm::inverse(_matrix3x3);
        return Matrix3x3(result);
    }

    bool Matrix3x3::IsIdentity() const {
        return *this == Matrix3x3::Identity;
    }

    Matrix3x3 Matrix3x3::GetTranspose() const {
        glm::mat3x3 result = glm::transpose(_matrix3x3);
        return Matrix3x3(result);
    }

    // MARK: Public Instance methods
    Vector3 Matrix3x3::GetColumn(int index) {
        return this->operator[](index);
    }

    Vector3 Matrix3x3::GetRow(int index) {
        float x = this->operator[](0)[index];
        float y = this->operator[](1)[index];
        float z = this->operator[](2)[index];
        return Vector3(x, y, z);
    }

    void Matrix3x3::SetColumn(int index, Vector3 column) {
        _matrix3x3[index][0] = column.GetX();
        _matrix3x3[index][1] = column.GetY();
        _matrix3x3[index][2] = column.GetZ();
    }

    void Matrix3x3::SetRow(int index, Vector3 row) {
        _matrix3x3[0][index] = row.GetX();
        _matrix3x3[1][index] = row.GetY();
        _matrix3x3[2][index] = row.GetZ();
    }

    float* Matrix3x3::Unsafe_AddressOf() {
        return &_matrix3x3[0].x;
    }

    VWOLF_VMATH_SERIALIZATION_MATRIX_EMITTER(Matrix3x3, 3, 3)
}
