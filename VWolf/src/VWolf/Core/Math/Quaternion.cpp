//
//  Quaternion.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 8/25/24.
//

#include "Quaternion.h"

namespace VWolf {
    const Quaternion Quaternion::Identity(1, 0, 0, 0);

    Quaternion::Quaternion(): quat(glm::quat(0, 0, 0, 0)), w(quat.w), x(quat.x), y(quat.y), z(quat.z) {}

    Quaternion::Quaternion(float w, float x, float y, float z): quat(glm::quat(w, x, y, z)), w(quat.w), x(quat.x), y(quat.y), z(quat.z) {}

    Quaternion::Quaternion(const Quaternion& quaternion): quat(glm::quat(quaternion.w, quaternion.x, quaternion.y, quaternion.z)), w(quat.w), x(quat.x), y(quat.y), z(quat.z) {}

    Quaternion::Quaternion(Quaternion& quaternion): quat(glm::quat(quaternion.w, quaternion.x, quaternion.y, quaternion.z)), w(quat.w), x(quat.x), y(quat.y), z(quat.z) {}

    Quaternion::Quaternion(Quaternion&& quaternion): quat(glm::quat(quaternion.w, quaternion.x, quaternion.y, quaternion.z)), w(quat.w), x(quat.x), y(quat.y), z(quat.z) {
        quaternion.w = 0;
        quaternion.x = 0;
        quaternion.y = 0;
        quaternion.z = 0;
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
        this->x = this->quat.x;
        this->y = this->quat.y;
        this->z = this->quat.z;
        this->w = this->quat.w;

        return *this;
    }

    // MARK: Operator overloading
    std::ostream& operator<<(std::ostream& os, const Quaternion& v) {
        os << "Quaternion(" << std::addressof(v) << ") - { w: " << v.w << ", x: " << v.x << ", y: " << v.y << ", z: " << v.z << "}";
        return os;
    }

    bool Quaternion::operator==(const Quaternion& rhs) {
        return this->quat == rhs.quat;
    }

    bool operator==(const Quaternion& lhs, const Quaternion& rhs) {
        return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z && lhs.w == rhs.w;;
    }

    Quaternion Quaternion::operator*(Quaternion rhs) {
        glm::quat quat = this->quat * rhs.quat;
        return Quaternion(quat.w, quat.x, quat.y, quat.z);
    }

    Quaternion operator*(const Quaternion& lhs, Quaternion rhs) {
        Quaternion q = lhs * rhs;
        return q;
    }

    // MARK: Get Functions
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
        glm::vec3 vec(fromTo.x, fromTo.y, fromTo.z);
        glm::quat quat = glm::quat(vec);
        this->x = quat.x;
        this->y = quat.y;
        this->z = quat.z;
        this->w = quat.w;
    }

    void Quaternion::SetLookRotation(Vector3 view, Vector3 up) {
        glm::vec3 direction(view.x, view.y, view.z);
        glm::vec3 _up(up.x, up.y, up.z);
        glm::quat quat = glm::quatLookAt(direction, _up);
        this->quat = quat;
        this->x = this->quat.x;
        this->y = this->quat.y;
        this->z = this->quat.z;
        this->w = this->quat.w;
    }

    void Quaternion::ToAngleAxis(float& angle, Vector3& axis) {
        glm::vec3 vec(axis.x, axis.y, axis.y);
        glm::quat quat = glm::angleAxis(angle, vec);
        this->quat = quat;
        this->x = this->quat.x;
        this->y = this->quat.y;
        this->z = this->quat.z;
        this->w = this->quat.w;
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
