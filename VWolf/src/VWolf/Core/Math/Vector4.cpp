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
    Vector4::Vector4(): _vector4(glm::vec4(0, 0, 0, 0)), x(_vector4.x), y(_vector4.y), z(_vector4.z), w(_vector4.w) {}

    Vector4::Vector4(float x, float y, float z, float w): _vector4(glm::vec4(x, y, z, w)), x(_vector4.x), y(_vector4.y), z(_vector4.z), w(_vector4.w)  {}

    Vector4::Vector4(Vector4& vector4): _vector4(glm::vec4(vector4._vector4.x, vector4._vector4.y, vector4._vector4.z, vector4._vector4.w)), x(_vector4.x), y(_vector4.y), z(_vector4.z), w(_vector4.w)  {}

    Vector4::Vector4(Vector4&& vector4): _vector4(std::move(vector4._vector4)), x(_vector4.x), y(_vector4.y), z(_vector4.z), w(_vector4.w) {
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
        this->x = this->_vector4.x;
        this->y = this->_vector4.y;
        this->z = this->_vector4.z;
        this->w = this->_vector4.w;

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
        os << "Vector4(" << std::addressof(v) << ") - { x: " << v.x << ", y: " << v.y << ", z: " << v.z << ", w: " << v.w << "}";
        return os;
    }

    bool Vector4::operator==(const Vector4& rhs) {
        return this->_vector4.x == rhs._vector4.x && this->_vector4.y == rhs._vector4.y && this->_vector4.z == rhs._vector4.z && this->_vector4.w == rhs._vector4.w;
    }

    bool operator==(const Vector4& lhs, const Vector4& rhs) {
        return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z && lhs.w == rhs.w;
    }

    bool Vector4::operator!=(const Vector4& rhs) {
        return this->_vector4.x != rhs._vector4.x || this->_vector4.y != rhs._vector4.y || this->_vector4.z != rhs._vector4.z || this->_vector4.w != rhs._vector4.w;
    }

    bool operator!=(const Vector4& lhs, const Vector4& rhs) {
        return lhs.x != rhs.x || lhs.y != rhs.y || lhs.z != rhs.z || lhs.w != rhs.w;
    }

    Vector4 Vector4::operator+(const Vector4& rhs) {
        glm::vec4 vec = this->_vector4 + rhs._vector4;
        return Vector4(vec.x, vec.y, vec.z, vec.w);
    }

    Vector4 operator+(const Vector4& lhs, const Vector4& rhs) {
        Vector4 result = lhs + rhs;
        return result;
    }

    Vector4 Vector4::operator-(const Vector4& rhs) {
        glm::vec4 vec = this->_vector4 - rhs._vector4;
        return Vector4(vec.x, vec.y, vec.z, vec.w);
    }

    Vector4 operator-(const Vector4& lhs, const Vector4& rhs) {
        Vector4 result = lhs - rhs;
        return result;
    }

    Vector4 Vector4::operator*(float rhs) {
        glm::vec4 vec = this->_vector4 * rhs;
        return Vector4(vec.x, vec.y, vec.z, vec.w);
    }

    Vector4 operator*(const Vector4& lhs, float rhs) {
        Vector4 result = lhs * rhs;
        return result;
    }

    Vector4 Vector4::operator/(float rhs) {
        glm::vec4 vec = this->_vector4 / rhs;
        return Vector4(vec.x, vec.y, vec.z, vec.w);
    }

    Vector4 operator/(const Vector4& lhs, float rhs) {
        Vector4 result = lhs / rhs;
        return result;
    }

    float Vector4::operator[](int index) {
        if (index < 0 || index > 3) return std::numeric_limits<float>::max();

        return _vector4[index];
    }

    Vector4::operator Vector2() {
        return Vector2(this->x, this->y);
    }

    Vector4::operator Vector3() {
        return Vector3(this->x, this->y, this->z);
    }

    // MARK: Get Functions
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
        return Vector4(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
    }

    Vector4 Vector4::ProjectOnPlane(Vector4 vector, Vector4 planeNormal) {
        glm::vec4 vec4 = glm::proj(vector._vector4, planeNormal._vector4);
        return Vector4(vec4.x, vec4.y, vec4.z, vec4.w);
    }
}
