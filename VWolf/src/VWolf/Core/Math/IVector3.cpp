//
//  IVector3.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 8/22/24.
//

#include "vwpch.h"
#include "IVector3.h"
#include "IVector2.h"
#include "Vector3.h"

namespace VWolf {
    // MARK: Constants
    const IVector3 IVector3::Up(0, 1, 0);

    const IVector3 IVector3::Down(0, -1, 0);

    const IVector3 IVector3::Right(1, 0, 0);

    const IVector3 IVector3::Left(-1, 0, 0);

    const IVector3 IVector3::Forward(0, 0, 1);

    const IVector3 IVector3::Back(0, 0, -1);

    const IVector3 IVector3::Zero(0, 0, 0);

    const IVector3 IVector3::One(1, 1, 1);

    // MARK: Constructors
    IVector3::IVector3(): _vector3(glm::ivec3(0, 0, 0)), x(_vector3.x), y(_vector3.y), z(_vector3.z) {}

    IVector3::IVector3(int32_t x, int32_t y, int32_t z): _vector3(glm::ivec3(x, y, z)), x(_vector3.x), y(_vector3.y), z(_vector3.z)  {}

    IVector3::IVector3(IVector3& vector3): _vector3(glm::ivec3(vector3._vector3.x, vector3._vector3.y, vector3._vector3.z)), x(_vector3.x), y(_vector3.y), z(_vector3.z)  {}

    IVector3::IVector3(IVector3&& vector3): _vector3(std::move(vector3._vector3)), x(_vector3.x), y(_vector3.y), z(_vector3.z)  {
        vector3._vector3.x = 0;
        vector3._vector3.y = 0;
        vector3._vector3.z = 0;
    }

    // MARK: Destructor
    IVector3::~IVector3() {
        this->_vector3.x = 0;
        this->_vector3.y = 0;
        this->_vector3.z = 0;
    }

    // MARK: Assignment operators
    IVector3& IVector3::operator=(const IVector3& other) {
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
    std::ostream& operator<<(std::ostream& os, const IVector3& v) {
        os << "IVector3(" << std::addressof(v) << ") - { x: " << v.x << ", y: " << v.y << ", z: " << v.z << "}";
        return os;
    }

    bool IVector3::operator==(const IVector3& rhs) {
        return this->_vector3.x == rhs._vector3.x && this->_vector3.y == rhs._vector3.y && this->_vector3.z == rhs._vector3.z;
    }

    bool operator==(const IVector3& lhs, const IVector3& rhs) {
        return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
    }

    bool IVector3::operator!=(const IVector3& rhs) {
        return this->_vector3.x != rhs._vector3.x || this->_vector3.y != rhs._vector3.y || this->_vector3.z != rhs._vector3.z;
    }

    bool operator!=(const IVector3& lhs, const IVector3& rhs) {
        return lhs.x != rhs.x || lhs.y != rhs.y || lhs.z != rhs.z;
    }

    IVector3 IVector3::operator+(const IVector3& rhs) {
        glm::ivec3 vec = this->_vector3 + rhs._vector3;
        return IVector3(vec.x, vec.y, vec.z);
    }

    IVector3 operator+(const IVector3& lhs, const IVector3& rhs) {
        IVector3 result = lhs + rhs;
        return result;
    }

    IVector3 IVector3::operator-(const IVector3& rhs) {
        glm::ivec3 vec = this->_vector3 - rhs._vector3;
        return IVector3(vec.x, vec.y, vec.z);
    }

    IVector3 operator-(const IVector3& lhs, const IVector3& rhs) {
        IVector3 result = lhs - rhs;
        return result;
    }

    IVector3 IVector3::operator*(int32_t rhs) {
        glm::ivec3 vec = this->_vector3 * rhs;
        return IVector3(vec.x, vec.y, vec.z);
    }

    IVector3 operator*(const IVector3& lhs, int32_t rhs) {
        IVector3 result = lhs * rhs;
        return result;
    }

    IVector3 IVector3::operator/(int32_t rhs) {
        glm::ivec3 vec = this->_vector3 / rhs;
        return IVector3(vec.x, vec.y, vec.z);
    }

    IVector3 operator/(const IVector3& lhs, int32_t rhs) {
        IVector3 result = lhs / rhs;
        return result;
    }

    int32_t IVector3::operator[](int index) {
        if (index < 0 || index > 2) return std::numeric_limits<int32_t>::max();

        return _vector3[index];
    }

    IVector3::operator IVector2() {
        return IVector2(this->x, this->y);
    }

    IVector3::operator Vector3() {
        return Vector3(this->x, this->y, this->z);
    }

    // MARK: Get Functions
    float IVector3::Magnitude() const {
        return _vector3.length();
    }

    float IVector3::SqrMagnitude() const {
        return glm::sqrt(Magnitude());
    }

    // MARK: Static Functions
    IVector3 IVector3::CeilToInt(Vector3 vector) {
        return IVector3(std::ceil(vector.x), std::ceil(vector.y),  std::ceil(vector.z));
    }

    float IVector3::Distance(IVector3 a, IVector3 b) {
        float result = Vector3::Distance((Vector3)a, (Vector3)b);
        return result;
    }

    IVector3 IVector3::FloorToInt(Vector3 vector) {
        return IVector3(std::floor(vector.x), std::floor(vector.y), std::floor(vector.z));
    }

    IVector3 IVector3::Max(IVector3 lhs, IVector3 rhs) {
        glm::ivec3 vec3 = glm::max(lhs._vector3, rhs._vector3);
        return IVector3(vec3.x, vec3.y, vec3.z);
    }

    IVector3 IVector3::Min(IVector3 lhs, IVector3 rhs) {
        glm::ivec3 vec3 = glm::min(lhs._vector3, rhs._vector3);
        return IVector3(vec3.x, vec3.y, vec3.z);
    }

    IVector3 IVector3::RoundToInt(Vector3 vector) {
        return IVector3(std::round(vector.x), std::round(vector.y), std::round(vector.z));
    }

    IVector3 IVector3::Scale(IVector3 a, IVector3 b) {
        return IVector3(a.x * b.x, a.y * b.y, a.z * b.z);
    }
}
