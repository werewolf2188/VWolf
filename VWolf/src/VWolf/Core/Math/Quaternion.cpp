//
//  Quaternion.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 8/25/24.
//

#include "vwpch.h"
#include "Quaternion.h"
#include "Matrix4x4.h"

#include <glm/gtx/euler_angles.hpp>

namespace VWolf {
    const Quaternion Quaternion::Identity(1, 0, 0, 0);

    Quaternion::Quaternion(): quat(glm::quat(0, 0, 0, 0)) {}

    Quaternion::Quaternion(float w, float x, float y, float z): quat(glm::quat(w, x, y, z)) {}

    Quaternion::Quaternion(glm::quat initializer): quat(std::move(initializer)) { }

    Quaternion::Quaternion(const Quaternion& quaternion): quat(glm::quat(quaternion.quat.w, quaternion.quat.x, quaternion.quat.y, quaternion.quat.z)) {}

    Quaternion::Quaternion(Quaternion& quaternion): quat(glm::quat(quaternion.quat.w, quaternion.quat.x, quaternion.quat.y, quaternion.quat.z)) {}

    Quaternion::Quaternion(Quaternion&& quaternion): quat(glm::quat(quaternion.quat.w, quaternion.quat.x, quaternion.quat.y, quaternion.quat.z)) {
        quaternion.quat.w = 0;
        quaternion.quat.x = 0;
        quaternion.quat.y = 0;
        quaternion.quat.z = 0;
    }

    Quaternion::~Quaternion() {
        quat.w = 0;
        quat.x = 0;
        quat.y = 0;
        quat.z = 0;
    }

    // MARK: Assignment operators
    Quaternion& Quaternion::operator=(const Quaternion& other) {
        this->quat = other.quat;

        return *this;
    }

    // MARK: Operator overloading
    std::ostream& operator<<(std::ostream& os, const Quaternion& v) {
        os << "Quaternion(" << std::addressof(v) << ") - { w: " << v.GetW() << ", x: " << v.GetX() << ", y: " << v.GetY() << ", z: " << v.GetZ() << "}";
        return os;
    }

    bool Quaternion::operator==(const Quaternion& rhs) {
        return this->quat == rhs.quat;
    }

    bool operator==(const Quaternion& lhs, const Quaternion& rhs) {
        return lhs.GetW() == rhs.GetW() && lhs.GetX() == rhs.GetX() && lhs.GetY() == rhs.GetY() && lhs.GetZ() == rhs.GetZ();
    }

    Quaternion Quaternion::operator*(Quaternion rhs) {
        glm::quat quat = this->quat * rhs.quat;
        return Quaternion(quat.w, quat.x, quat.y, quat.z);
    }

    Quaternion operator*(const Quaternion& lhs, Quaternion rhs) {
        glm::quat quat = lhs.quat * rhs.quat;
        return Quaternion(quat.w, quat.x, quat.y, quat.z);
    }

    // MARK: Get Functions
    const float Quaternion::GetW() const { return quat.w; }

    float& Quaternion::GetW() { return quat.w; }

    const float Quaternion::GetX() const { return quat.x; }

    float& Quaternion::GetX() { return quat.x; }

    const float Quaternion::GetY() const { return quat.y; }

    float& Quaternion::GetY() { return quat.y; }

    const float Quaternion::GetZ() const { return quat.z; }

    float& Quaternion::GetZ() { return quat.z; }

    void Quaternion::SetW(float value) { quat.w = value; }

    void Quaternion::SetX(float value) { quat.x = value; }

    void Quaternion::SetY(float value) { quat.y = value; }

    void Quaternion::SetZ(float value) { quat.z = value; }

    Vector3 Quaternion::EulerAngles() const {
        glm::vec3 vec3 = glm::eulerAngles(this->quat);
        return Vector3(vec3.x, vec3.y, vec3.z);
    }

    Quaternion Quaternion::Normalized() const {
        glm::quat quat = glm::normalize(this->quat);
        return Quaternion(quat.w, quat.x, quat.y, quat.z);
    }

    void Quaternion::SetFromToRotation(Vector3 from, Vector3 to) {
        Vector3 fromTo = to - from;
        glm::vec3 vec(fromTo.GetX(), fromTo.GetY(), fromTo.GetZ());
        glm::quat quat = glm::quat(vec);
        this->quat = quat;
    }

    void Quaternion::SetLookRotation(Vector3 view, Vector3 up) {
        glm::vec3 direction(view.GetX(), view.GetY(), view.GetZ());
        glm::vec3 _up(up.GetX(), up.GetY(), up.GetZ());
        glm::quat quat = glm::quatLookAt(direction, _up);
        this->quat = quat;
    }

    void Quaternion::ToAngleAxis(float& angle, Vector3& axis) {
        glm::vec3 vec(axis.GetX(), axis.GetY(), axis.GetZ());
        glm::quat quat = glm::angleAxis(angle, vec);
        this->quat = quat;
    }

    Vector3 Quaternion::GetOrientation(Vector3 vector) {
        glm::vec3 vec = glm::rotate(quat, vector._vector3);
        return Vector3(vec);
    }

    Matrix4x4 Quaternion::ToMatrix4x4() const {
        glm::mat4x4 matrix = toMat4(quat);
        return Matrix4x4(matrix);
    }

    // MARK: Static Functions
    float Quaternion::Angle(Quaternion from, Quaternion to) {
        glm::quat q3 = glm::conjugate(to.quat);
        glm::quat q_multiply = from.quat * q3;
        float acosW = glm::acos(q_multiply.w);
        float angle = glm::degrees((2 * acosW));

        return angle;
    }

    Quaternion Quaternion::AngleAxis(float angle, Vector3 axis) {
        Quaternion q;
        q.ToAngleAxis(angle, axis);
        return q;
    }

    float Quaternion::Dot(Quaternion lhs, Quaternion rhs) {
        return glm::dot(lhs.quat, rhs.quat);
    }

    Quaternion Quaternion::Euler(float x, float y, float z) {
        glm::vec3 vec(x, y, z);
        glm::quat quat = glm::quat(vec);
        return Quaternion(quat.w, quat.x, quat.y, quat.z);
    }

    Quaternion Quaternion::FromToRotation(Vector3 from, Vector3 to) {
        Quaternion q;
        q.SetFromToRotation(from, to);
        return q;
    }

    Quaternion Quaternion::Inverse(Quaternion quaternion) {
        glm::quat quat = quaternion.quat;
        quat = glm::inverse(quat);
        return Quaternion(quat.w, quat.x, quat.y, quat.z);
    }

    Quaternion Quaternion::Lerp(Quaternion a, Quaternion b, float t) {
        glm::quat aa = a.quat;
        glm::quat bb = b.quat;
        glm::quat quat = glm::lerp(aa, bb, t);
        return Quaternion(quat.w, quat.x, quat.y, quat.z);
    }

    Quaternion Quaternion::LerpUnclamped(Quaternion a, Quaternion b, float t) {
        glm::quat aa = a.quat;
        glm::quat bb = b.quat;
        glm::quat quat = glm::lerp(aa, bb, t);
        return Quaternion(quat.w, quat.x, quat.y, quat.z);
    }

    Quaternion Quaternion::Normalize(Quaternion quaternion) {
        glm::quat quat = quaternion.quat;
        quat = glm::normalize(quat);
        return Quaternion(quat.w, quat.x, quat.y, quat.z);
    }

    Quaternion Quaternion::RotateTowards(Vector3 from, Vector3 to, float maxDegreesDelta) {
        throw std::exception();
    }

    Quaternion Quaternion::Slerp(Quaternion a, Quaternion b, float t) {
        glm::quat aa = a.quat;
        glm::quat bb = b.quat;
        glm::quat quat = glm::slerp(aa, bb, t);
        return Quaternion(quat.w, quat.x, quat.y, quat.z);
    }

    Quaternion Quaternion::SlerpUnclamped(Quaternion a, Quaternion b, float t) {
        glm::quat aa = a.quat;
        glm::quat bb = b.quat;
        glm::quat quat = glm::slerp(aa, bb, t);
        return Quaternion(quat.w, quat.x, quat.y, quat.z);
    }
}
