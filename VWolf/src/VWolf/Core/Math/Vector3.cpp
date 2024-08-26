//
//  Vector3.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 8/22/24.
//

#include "vwpch.h"
#include "Vector3.h"
#include "Vector2.h"
#include "Vector4.h"

#include <glm/gtx/perpendicular.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/projection.hpp>

namespace VWolf {
    // MARK: Constants
    const Vector3 Vector3::Up(0, 1, 0);

    const Vector3 Vector3::Down(0, -1, 0);

    const Vector3 Vector3::Right(1, 0, 0);

    const Vector3 Vector3::Left(-1, 0, 0);

    const Vector3 Vector3::Forward(0, 0, 1);

    const Vector3 Vector3::Back(0, 0, -1);

    const Vector3 Vector3::Zero(0, 0, 0);

    const Vector3 Vector3::One(1, 1, 1);

    const Vector3 Vector3::NegativeInfinity(std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), std::numeric_limits<float>::min());

    const Vector3 Vector3::PositiveInfinity(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());

    // MARK: Constructors
    Vector3::Vector3(): _vector3(glm::vec3(0, 0, 0)), x(_vector3.x), y(_vector3.y), z(_vector3.z) {}

    Vector3::Vector3(float x, float y, float z): _vector3(glm::vec3(x, y, z)), x(_vector3.x), y(_vector3.y), z(_vector3.z)  {}

    Vector3::Vector3(Vector3& Vector3): _vector3(glm::vec3(Vector3._vector3.x, Vector3._vector3.y, Vector3._vector3.z)), x(_vector3.x), y(_vector3.y), z(_vector3.z)  {}

    Vector3::Vector3(const Vector3& Vector3): _vector3(glm::vec3(Vector3._vector3.x, Vector3._vector3.y, Vector3._vector3.z)), x(_vector3.x), y(_vector3.y), z(_vector3.z)  {}

    Vector3::Vector3(Vector3&& Vector3): _vector3(std::move(Vector3._vector3)), x(_vector3.x), y(_vector3.y), z(_vector3.z)  {
        Vector3._vector3.x = 0;
        Vector3._vector3.y = 0;
        Vector3._vector3.z = 0;
    }

    // MARK: Destructor
    Vector3::~Vector3() {
        this->_vector3.x = 0;
        this->_vector3.y = 0;
        this->_vector3.z = 0;
    }

    // MARK: Assignment operators
    Vector3& Vector3::operator=(const Vector3& other) {
        this->_vector3 = other._vector3;
        this->x = this->_vector3.x;
        this->y = this->_vector3.y;
        this->z = this->_vector3.z;

        return *this;
    }

//    Vector3& Vector3::operator=(Vector3&& other) {
//        this->_vector3 = other._vector3;
//        this->x = this->_vector3.x;
//        this->y = this->_vector3.y;
//        this->z = this->_vector3.z;
//
//        other._vector3.x = 0;
//        other._vector3.y = 0;
//        other._vector3.z = 0;
//        return *this;
//    }

    // MARK: Operator overloading
    std::ostream& operator<<(std::ostream& os, const Vector3& v) {
        os << "Vector3(" << std::addressof(v) << ") - { x: " << v.x << ", y: " << v.y << ", z: " << v.z << "}";
        return os;
    }

    bool Vector3::operator==(const Vector3& rhs) {
        return this->_vector3.x == rhs._vector3.x && this->_vector3.y == rhs._vector3.y && this->_vector3.z == rhs._vector3.z;
    }

    bool operator==(const Vector3& lhs, const Vector3& rhs) {
        return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
    }

    bool Vector3::operator!=(const Vector3& rhs) {
        return this->_vector3.x != rhs._vector3.x || this->_vector3.y != rhs._vector3.y || this->_vector3.z != rhs._vector3.z;
    }

    bool operator!=(const Vector3& lhs, const Vector3& rhs) {
        return lhs.x != rhs.x || lhs.y != rhs.y || lhs.z != rhs.z;
    }

    Vector3 Vector3::operator+(const Vector3& rhs) {
        glm::vec3 vec = this->_vector3 + rhs._vector3;
        return Vector3(vec.x, vec.y, vec.z);
    }

    Vector3 operator+(const Vector3& lhs, const Vector3& rhs) {
        Vector3 result = lhs + rhs;
        return result;
    }

    Vector3 Vector3::operator-(const Vector3& rhs) {
        glm::vec3 vec = this->_vector3 - rhs._vector3;
        return Vector3(vec.x, vec.y, vec.z);
    }

    Vector3 operator-(const Vector3& lhs, const Vector3& rhs) {
        Vector3 result = lhs - rhs;
        return result;
    }

    Vector3 Vector3::operator*(float rhs) {
        glm::vec3 vec = this->_vector3 * rhs;
        return Vector3(vec.x, vec.y, vec.z);
    }

    Vector3 operator*(const Vector3& lhs, float rhs) {
        Vector3 result = lhs * rhs;
        return result;
    }

    Vector3 Vector3::operator/(float rhs) {
        glm::vec3 vec = this->_vector3 / rhs;
        return Vector3(vec.x, vec.y, vec.z);
    }

    Vector3 operator/(const Vector3& lhs, float rhs) {
        Vector3 result = lhs / rhs;
        return result;
    }

    float Vector3::operator[](int index) {
        if (index < 0 || index > 2) return std::numeric_limits<float>::max();

        return _vector3[index];
    }

    Vector3::operator Vector2() {
        return Vector2(this->x, this->y);
    }

    Vector3::operator Vector4() {
        return Vector4(this->x, this->y, this->z, 0);
    }

    // MARK: Get Functions
    Vector3 Vector3::Normalized() const {
        glm::vec3 normal = glm::normalize(_vector3);
        return Vector3(normal.x, normal.y, normal.z);
    }

    void Vector3::Normalize() {
        _vector3 = glm::normalize(_vector3);
    }

    float Vector3::Magnitude() const {
        return _vector3.length();
    }

    float Vector3::SqrMagnitude() const {
        return glm::sqrt(Magnitude());
    }

    // MARK: Static Functions
    float Vector3::Angle(Vector3 from, Vector3 to) {
        float angle = glm::abs(glm::angle(from._vector3, to._vector3));
        return angle;
    }

    Vector3 Vector3::ClampMagnitude(Vector3 vector, float maxLength) {
        glm::vec3 vec3 = glm::clamp(vector._vector3, 0.0f, maxLength);
        return Vector3(vec3.x, vec3.y, vec3.z);
    }

    Vector3 Vector3::Cross(Vector3 a, Vector3 b) {
        glm::vec3 vec3 = glm::cross(a._vector3, b._vector3);
        return Vector3(vec3.x, vec3.y, vec3.z);
    }

    float Vector3::Distance(Vector3 a, Vector3 b) {
        return glm::distance(a._vector3, b._vector3);
    }

    float Vector3::Dot(Vector3 lhs, Vector3 rhs) {
        return glm::dot(lhs._vector3, rhs._vector3);
    }

    Vector3 Vector3::Lerp(Vector3 a, Vector3 b, float t) {
        Vector3 result = a + (b - a) * t;
        return ClampMagnitude(result, 1);
    }

    Vector3 Vector3::LerpUnclamped(Vector3 a, Vector3 b, float t) {
        Vector3 result = a + (b - a) * t;
        return result;
    }

    Vector3 Vector3::Max(Vector3 lhs, Vector3 rhs) {
        glm::vec3 vec3 = glm::max(lhs._vector3, rhs._vector3);
        return Vector3(vec3.x, vec3.y, vec3.z);
    }

    Vector3 Vector3::Min(Vector3 lhs, Vector3 rhs) {
        glm::vec3 vec3 = glm::min(lhs._vector3, rhs._vector3);
        return Vector3(vec3.x, vec3.y, vec3.z);
    }

    Vector3 Vector3::MoveTowards(Vector3 current, Vector3 target, float maxDistanceDelta) {
        Vector3 result = current + (target - current) * maxDistanceDelta;
        glm::vec3 vec3 = glm::clamp(result._vector3, 0.0f, maxDistanceDelta);
        return Vector3(vec3.x, vec3.y, vec3.z);
    }

    Vector3 Vector3::Perpendicular(Vector3 inDirection) {
        glm::vec3 vec3 = glm::min(Up._vector3, inDirection._vector3);
        return Vector3(vec3.x, vec3.y, vec3.z);
    }

    Vector3 Vector3::Reflect(Vector3 inDirection, Vector3 inNormal) {
        glm::vec3 vec3 = glm::reflect(inDirection._vector3, inNormal._vector3);
        return Vector3(vec3.x, vec3.y, vec3.z);
    }

    Vector3 Vector3::Scale(Vector3 a, Vector3 b) {
        return Vector3(a.x * b.x, a.y * b.y, a.z * b.z);
    }

    float Vector3::SignedAngle(Vector3 from, Vector3 to) {
        return glm::angle(from._vector3, to._vector3);
    }

    Vector3 Vector3::Project(Vector3 vector, Vector3 onNormal) {
        return Scale(onNormal, vector);
    }

    Vector3 Vector3::ProjectOnPlane(Vector3 vector, Vector3 planeNormal) {
        glm::vec3 vec3 = glm::proj(vector._vector3, planeNormal._vector3);
        return Vector3(vec3.x, vec3.y, vec3.z);
    }

    Vector3 Vector3::Slerp(Vector3 a, Vector3 b, float t) {
        // Dot product - the cosine of the angle between 2 vectors.
        float dot = Dot(a, b);

        // Clamp it to be in the range of Acos()
        // This may be unnecessary, but floating point
        // precision can be a fickle mistress.
        float clamp = glm::clamp(dot, -1.0f, 1.0f);

        // Acos(dot) returns the angle between start and end,
        // And multiplying that by percent returns the angle between
        // start and the final result.
        float theta = glm::acos(clamp) * t;
        Vector3 relative = b - a * dot;
        relative.Normalize();

        // Orthonormal basis
        // The final result.
        return ((a * glm::cos(theta)) + (relative * glm::sin(theta)));
    }

    Vector3 Vector3::SlerpUnclamped(Vector3 a, Vector3 b, float t) {
        throw std::exception();
    }
}
