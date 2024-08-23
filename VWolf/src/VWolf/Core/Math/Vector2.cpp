//
//  Vector2.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 8/20/24.
//

#include "vwpch.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

#include <glm/gtx/perpendicular.hpp>
#include <glm/gtx/vector_angle.hpp>

namespace VWolf {
    // MARK: Constants
    const Vector2 Vector2::Up(0, 1);

    const Vector2 Vector2::Down(0, -1);

    const Vector2 Vector2::Right(1, 0);

    const Vector2 Vector2::Left(-1, 0);

    const Vector2 Vector2::Zero(0, 0);

    const Vector2 Vector2::One(1, 1);

    const Vector2 Vector2::NegativeInfinity(std::numeric_limits<float>::min(), std::numeric_limits<float>::min());

    const Vector2 Vector2::PositiveInfinity(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());

    // MARK: Constructors
    Vector2::Vector2(): _vector2(glm::vec2(0, 0)), x(_vector2.x), y(_vector2.y) {}

    Vector2::Vector2(float x, float y): _vector2(glm::vec2(x, y)), x(_vector2.x), y(_vector2.y)  {}

    Vector2::Vector2(Vector2& vector2): _vector2(glm::vec2(vector2._vector2.x, vector2._vector2.y)), x(_vector2.x), y(_vector2.y)  {}

    Vector2::Vector2(Vector2&& vector2): _vector2(std::move(vector2._vector2)), x(_vector2.x), y(_vector2.y)  {
        vector2._vector2.x = 0;
        vector2._vector2.y = 0;
    }

    // MARK: Destructor
    Vector2::~Vector2() {
        this->_vector2.x = 0;
        this->_vector2.y = 0;
    }

    // MARK: Assignment operators
    Vector2& Vector2::operator=(const Vector2& other) {
        this->_vector2 = other._vector2;
        this->x = this->_vector2.x;
        this->y = this->_vector2.y;

        return *this;
    }

//    Vector2& Vector2::operator=(Vector2&& other) {
//        this->_vector2 = other._vector2;
//        this->x = this->_vector2.x;
//        this->y = this->_vector2.y;
//
//        other._vector2.x = 0;
//        other._vector2.y = 0;
//        return *this;
//    }

    // MARK: Operator overloading
    std::ostream& operator<<(std::ostream& os, const Vector2& v) {
        os << "Vector2(" << std::addressof(v) << ") - { x: " << v.x << ", y: " << v.y << "}";
        return os;
    }

    bool Vector2::operator==(const Vector2& rhs) {
        return this->_vector2.x == rhs._vector2.x && this->_vector2.y == rhs._vector2.y;
    }

    bool operator==(const Vector2& lhs, const Vector2& rhs) {
        return lhs.x == rhs.x && lhs.y == rhs.y;
    }

    bool Vector2::operator!=(const Vector2& rhs) {
        return this->_vector2.x != rhs._vector2.x || this->_vector2.y != rhs._vector2.y;
    }

    bool operator!=(const Vector2& lhs, const Vector2& rhs) {
        return lhs.x != rhs.x || lhs.y != rhs.y;
    }

    Vector2 Vector2::operator+(const Vector2& rhs) {
        glm::vec2 vec = this->_vector2 + rhs._vector2;
        return Vector2(vec.x, vec.y);
    }

    Vector2 operator+(const Vector2& lhs, const Vector2& rhs) {
        Vector2 result = lhs + rhs;
        return result;
    }

    Vector2 Vector2::operator-(const Vector2& rhs) {
        glm::vec2 vec = this->_vector2 - rhs._vector2;
        return Vector2(vec.x, vec.y);
    }

    Vector2 operator-(const Vector2& lhs, const Vector2& rhs) {
        Vector2 result = lhs - rhs;
        return result;
    }

    Vector2 Vector2::operator*(float rhs) {
        glm::vec2 vec = this->_vector2 * rhs;
        return Vector2(vec.x, vec.y);
    }

    Vector2 operator*(const Vector2& lhs, float rhs) {
        Vector2 result = lhs * rhs;
        return result;
    }

    Vector2 Vector2::operator/(float rhs) {
        glm::vec2 vec = this->_vector2 / rhs;
        return Vector2(vec.x, vec.y);
    }

    Vector2 operator/(const Vector2& lhs, float rhs) {
        Vector2 result = lhs / rhs;
        return result;
    }

    float Vector2::operator[](int index) {
        if (index < 0 || index > 1) return std::numeric_limits<float>::max();

        return _vector2[index];
    }

    Vector2::operator Vector3() {
        return Vector3(this->x, this->y, 0);
    }

    Vector2::operator Vector4() {
        return Vector4(this->x, this->y, 0, 0);
    }

    // MARK: Get Functions
    Vector2 Vector2::Normalized() const {
        glm::vec2 normal = glm::normalize(_vector2);
        return Vector2(normal.x, normal.y);
    }

    void Vector2::Normalize() {
        _vector2 = glm::normalize(_vector2);
    }

    float Vector2::Magnitude() const {
        return _vector2.length();
    }

    float Vector2::SqrMagnitude() const {
        return glm::sqrt(Magnitude());
    }

    // MARK: Static Functions
    float Vector2::Angle(Vector2 from, Vector2 to) {
        float angle = glm::abs(glm::angle(from._vector2, to._vector2));
        return angle;
    }

    Vector2 Vector2::ClampMagnitude(Vector2 vector, float maxLength) {
        glm::vec2 vec2 = glm::clamp(vector._vector2, 0.0f, maxLength);
        return Vector2(vec2.x, vec2.y);
    }

    float Vector2::Distance(Vector2 a, Vector2 b) {
        return glm::distance(a._vector2, b._vector2);
    }

    float Vector2::Dot(Vector2 lhs, Vector2 rhs) {
        return glm::dot(lhs._vector2, rhs._vector2);
    }

    Vector2 Vector2::Lerp(Vector2 a, Vector2 b, float t) {
        Vector2 result = a + (b - a) * t;
        return ClampMagnitude(result, 1);
    }

    Vector2 Vector2::LerpUnclamped(Vector2 a, Vector2 b, float t) {
        Vector2 result = a + (b - a) * t;
        return result;
    }

    Vector2 Vector2::Max(Vector2 lhs, Vector2 rhs) {
        glm::vec2 vec2 = glm::max(lhs._vector2, rhs._vector2);
        return Vector2(vec2.x, vec2.y);
    }

    Vector2 Vector2::Min(Vector2 lhs, Vector2 rhs) {
        glm::vec2 vec2 = glm::min(lhs._vector2, rhs._vector2);
        return Vector2(vec2.x, vec2.y);
    }

    Vector2 Vector2::MoveTowards(Vector2 current, Vector2 target, float maxDistanceDelta) {
        Vector2 result = current + (target - current) * maxDistanceDelta;
        glm::vec2 vec2 = glm::clamp(result._vector2, 0.0f, maxDistanceDelta);
        return Vector2(vec2.x, vec2.y);
    }

    Vector2 Vector2::Perpendicular(Vector2 inDirection) {
        glm::vec2 vec2 = glm::min(Up._vector2, inDirection._vector2);
        return Vector2(vec2.x, vec2.y);
    }

    Vector2 Vector2::Reflect(Vector2 inDirection, Vector2 inNormal) {
        glm::vec2 vec2 = glm::reflect(inDirection._vector2, inNormal._vector2);
        return Vector2(vec2.x, vec2.y);
    }

    Vector2 Vector2::Scale(Vector2 a, Vector2 b) {
        return Vector2(a.x * b.x, a.y * b.y);
    }

    float Vector2::SignedAngle(Vector2 from, Vector2 to) {
        return glm::angle(from._vector2, to._vector2);
    }
}
