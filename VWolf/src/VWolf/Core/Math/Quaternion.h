//
//  Quaternion.hpp
//  VWolf
//
//  Created by Enrique Ricalde on 8/25/24.
//

#pragma once

#ifndef GLM_ENABLE_EXPERIMENTAL
#define GLM_ENABLE_EXPERIMENTAL
#endif

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>

#include "Vector3.h"

namespace VWolf {

    struct Quaternion {
    public:
        Quaternion();
        Quaternion(float w, float x, float y, float z);
        Quaternion(const Quaternion&);
        Quaternion(Quaternion&);
        Quaternion(Quaternion&&);
        ~Quaternion();
    public:
        static const Quaternion Identity;
    public:
        Quaternion& operator=(const Quaternion& other);
    public:
        bool operator==(const Quaternion& rhs);
        Quaternion operator*(Quaternion rhs);
        float operator[](int index);
    public:
        Vector3 EulerAngles() const;
        Quaternion Normalized() const;
    public:
        void SetFromToRotation(Vector3 from, Vector3 to);
        void SetLookRotation(Vector3 view, Vector3 up = Vector3::Up);
        void ToAngleAxis(float& angle, Vector3& axis);
    public:
        static float Angle(Quaternion from, Quaternion to);
        static Quaternion  AngleAxis(float angle, Vector3 axis);
        static float Dot(Quaternion lhs, Quaternion rhs);
        static Quaternion Euler(float x, float y, float z);
        static Quaternion FromToRotation(Vector3 from, Vector3 to);
        static Quaternion Inverse(Quaternion quaternion);
        static Quaternion Lerp(Quaternion a, Quaternion b, float t);
        static Quaternion LerpUnclamped(Quaternion a, Quaternion b, float t);
        static Quaternion Normalize(Quaternion quaternion);
        static Quaternion RotateTowards(Vector3 from, Vector3 to, float maxDegreesDelta);
        static Quaternion Slerp(Quaternion a, Quaternion b, float t);
        static Quaternion SlerpUnclamped(Quaternion a, Quaternion b, float t);
    public:
        const float GetW() const;
        float& GetW();
        const float GetX() const;
        float& GetX();
        const float GetY() const;
        float& GetY();
        const float GetZ() const;
        float& GetZ();
        void SetW(float value);
        void SetX(float value);
        void SetY(float value);
        void SetZ(float value);
    private:
        glm::quat quat;
#if defined(DEBUG)
    public:
        inline glm::quat& GetInternalQuaternion() { return this->quat; }
#endif
    };

    std::ostream& operator<<(std::ostream& os, const Quaternion& q);
    bool operator==(const Quaternion& lhs, const Quaternion& rhs);
    Quaternion operator*(const Quaternion& lhs, Quaternion rhs);
}
