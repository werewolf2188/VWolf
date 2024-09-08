//
//  Vector4.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 8/22/24.
//

#include "vwpch.h"
#include "Vector4.h"
#include "Vector3.h"
#include "Vector2.h"

#include <glm/gtx/perpendicular.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/projection.hpp>

namespace VWolf {
    // MARK: Constants
    const Vector4 Vector4::Zero(0, 0, 0, 0);

    const Vector4 Vector4::One(1, 1, 1, 1);

    const Vector4 Vector4::NegativeInfinity(std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), std::numeric_limits<float>::min());

    const Vector4 Vector4::PositiveInfinity(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());

    // MARK: Constructors
    Vector4::Vector4(): _vector4(glm::vec4(0, 0, 0, 0)) {}

    Vector4::Vector4(float x, float y, float z, float w): _vector4(glm::vec4(x, y, z, w)) {}

    Vector4::Vector4(glm::vec4 initializer): _vector4(std::move(initializer)) { }

    Vector4::Vector4(Vector4& vector4): _vector4(glm::vec4(vector4._vector4.x, vector4._vector4.y, vector4._vector4.z, vector4._vector4.w)) {}

    Vector4::Vector4(const Vector4& vector4): _vector4(glm::vec4(vector4._vector4.x, vector4._vector4.y, vector4._vector4.z, vector4._vector4.w)) {}

    Vector4::Vector4(Vector4&& vector4): _vector4(std::move(vector4._vector4)) {
        vector4._vector4.x = 0;
        vector4._vector4.y = 0;
        vector4._vector4.z = 0;
        vector4._vector4.w = 0;
    }

    // MARK: Destructor
    Vector4::~Vector4() {
        this->_vector4.x = 0;
        this->_vector4.y = 0;
        this->_vector4.z = 0;
        this->_vector4.z = 0;
    }

    // MARK: Assignment operators
    Vector4& Vector4::operator=(const Vector4& other) {
        this->_vector4 = other._vector4;

        return *this;
    }

//    Vector4& Vector4::operator=(Vector4&& other) {
//        this->_vector4 = other._vector4;
//        this->x = this->_vector4.x;
//        this->y = this->_vector4.y;
//        this->z = this->_vector4.z;
//
//        other._vector4.x = 0;
//        other._vector4.y = 0;
//        other._vector4.z = 0;
//        return *this;
//    }

    // MARK: Operator overloading
    std::ostream& operator<<(std::ostream& os, const Vector4& v) {
        os << "Vector4(" << std::addressof(v) << ") - { x: " << v.GetX() << ", y: " << v.GetY() << ", z: " << v.GetZ() << ", w: " << v.GetW() << "}";
        return os;
    }

    bool Vector4::operator==(const Vector4& rhs) {
        return this->_vector4.x == rhs._vector4.x && this->_vector4.y == rhs._vector4.y && this->_vector4.z == rhs._vector4.z && this->_vector4.w == rhs._vector4.w;
    }

    bool operator==(const Vector4& lhs, const Vector4& rhs) {
        return lhs.GetX() == rhs.GetX() && lhs.GetY() == rhs.GetY() && lhs.GetZ() == rhs.GetZ() && lhs.GetW() == rhs.GetW();
    }

    bool Vector4::operator!=(const Vector4& rhs) {
        return this->_vector4.x != rhs._vector4.x || this->_vector4.y != rhs._vector4.y || this->_vector4.z != rhs._vector4.z || this->_vector4.w != rhs._vector4.w;
    }

    bool operator!=(const Vector4& lhs, const Vector4& rhs) {
        return lhs.GetX() != rhs.GetX() || lhs.GetY() != rhs.GetY() || lhs.GetZ() != rhs.GetZ() || lhs.GetW() != rhs.GetW();
    }

    Vector4 Vector4::operator+(const Vector4& rhs) {
        glm::vec4 vec = this->_vector4 + rhs._vector4;
        return Vector4(vec.x, vec.y, vec.z, vec.w);
    }

    Vector4 operator+(const Vector4& lhs, const Vector4& rhs) {
        glm::vec4 vec = lhs._vector4 + rhs._vector4;
        return Vector4(vec.x, vec.y, vec.z, vec.w);
    }

    Vector4 Vector4::operator-(const Vector4& rhs) {
        glm::vec4 vec = this->_vector4 - rhs._vector4;
        return Vector4(vec.x, vec.y, vec.z, vec.w);
    }

    Vector4 operator-(const Vector4& lhs, const Vector4& rhs) {
        glm::vec4 vec = lhs._vector4 - rhs._vector4;
        return Vector4(vec.x, vec.y, vec.z, vec.w);
    }

    Vector4 Vector4::operator*(float rhs) {
        glm::vec4 vec = this->_vector4 * rhs;
        return Vector4(vec.x, vec.y, vec.z, vec.w);
    }

    Vector4 operator*(const Vector4& lhs, float rhs) {
        glm::vec4 vec = lhs._vector4 * rhs;
        return Vector4(vec.x, vec.y, vec.z, vec.w);
    }

    Vector4 operator*(float lhs, const Vector4& rhs) {
        glm::vec4 vec = lhs * rhs._vector4;
        return Vector4(vec.x, vec.y, vec.z, vec.w);
    }

    Vector4 Vector4::operator/(float rhs) {
        glm::vec4 vec = this->_vector4 / rhs;
        return Vector4(vec.x, vec.y, vec.z, vec.w);
    }

    Vector4 operator/(const Vector4& lhs, float rhs) {
        glm::vec4 vec = lhs._vector4 / rhs;
        return Vector4(vec.x, vec.y, vec.z, vec.w);
    }

    Vector4 operator/(float lhs, const Vector4& rhs) {
        glm::vec4 vec = lhs / rhs._vector4;
        return Vector4(vec.x, vec.y, vec.z, vec.w);
    }

    float Vector4::operator[](int index) {
        if (index < 0 || index > 3) return std::numeric_limits<float>::max();

        return _vector4[index];
    }

    Vector4::operator Vector2() {
        return Vector2(this->_vector4.x, this->_vector4.y);
    }

    Vector4::operator Vector3() {
        return Vector3(this->_vector4.x, this->_vector4.y, this->_vector4.z);
    }

    // MARK: Get Functions
    const float Vector4::GetX() const { return _vector4.x; }

    float& Vector4::GetX() { return _vector4.x; }

    const float Vector4::GetY() const { return _vector4.y; }

    float& Vector4::GetY() { return _vector4.y; }

    const float Vector4::GetZ() const { return _vector4.z; }

    float& Vector4::GetZ() { return _vector4.z; }

    const float Vector4::GetW() const { return _vector4.w; }

    float& Vector4::GetW() { return _vector4.w; }

    void Vector4::SetX(float value) { _vector4.x = value; }

    void Vector4::SetY(float value) { _vector4.y = value; }

    void Vector4::SetZ(float value) { _vector4.z = value; }

    void Vector4::SetW(float value) { _vector4.w = value; }

    Vector4 Vector4::Normalized() const {
        glm::vec4 normal = glm::normalize(_vector4);
        return Vector4(normal.x, normal.y, normal.z, normal.w);
    }

    void Vector4::Normalize() {
        _vector4 = glm::normalize(_vector4);
    }

    float Vector4::Magnitude() const {
        return _vector4.length();
    }

    float Vector4::SqrMagnitude() const {
        return glm::sqrt(Magnitude());
    }

    // MARK: Static Functions
    float Vector4::Distance(Vector4 a, Vector4 b) {
        return glm::distance(a._vector4, b._vector4);
    }

    float Vector4::Dot(Vector4 lhs, Vector4 rhs) {
        return glm::dot(lhs._vector4, rhs._vector4);
    }

    Vector4 Vector4::Lerp(Vector4 a, Vector4 b, float t) {
        Vector4 result = a + (b - a) * t;
        glm::vec4 vec4 = glm::clamp(result._vector4, 0.0f, 1.0f);
        return Vector4(vec4.x, vec4.y, vec4.z, vec4.w);
    }

    Vector4 Vector4::LerpUnclamped(Vector4 a, Vector4 b, float t) {
        Vector4 result = a + (b - a) * t;
        return result;
    }

    Vector4 Vector4::Max(Vector4 lhs, Vector4 rhs) {
        glm::vec4 vec4 = glm::max(lhs._vector4, rhs._vector4);
        return Vector4(vec4.x, vec4.y, vec4.z, vec4.w);
    }

    Vector4 Vector4::Min(Vector4 lhs, Vector4 rhs) {
        glm::vec4 vec4 = glm::min(lhs._vector4, rhs._vector4);
        return Vector4(vec4.x, vec4.y, vec4.z, vec4.w);
    }

    Vector4 Vector4::MoveTowards(Vector4 current, Vector4 target, float maxDistanceDelta) {
        Vector4 result = current + (target - current) * maxDistanceDelta;
        glm::vec4 vec4 = glm::clamp(result._vector4, 0.0f, maxDistanceDelta);
        return Vector4(vec4.x, vec4.y, vec4.z, vec4.w);
    }

    Vector4 Vector4::Reflect(Vector4 inDirection, Vector4 inNormal) {
        glm::vec4 vec4 = glm::reflect(inDirection._vector4, inNormal._vector4);
        return Vector4(vec4.x, vec4.y, vec4.z, vec4.w);
    }

    Vector4 Vector4::Scale(Vector4 a, Vector4 b) {
        return Vector4(a._vector4.x * b._vector4.x, a._vector4.y * b._vector4.y, a._vector4.z * b._vector4.z, a._vector4.w * b._vector4.w);
    }

    Vector4 Vector4::ProjectOnPlane(Vector4 vector, Vector4 planeNormal) {
        glm::vec4 vec4 = glm::proj(vector._vector4, planeNormal._vector4);
        return Vector4(vec4.x, vec4.y, vec4.z, vec4.w);
    }
}
