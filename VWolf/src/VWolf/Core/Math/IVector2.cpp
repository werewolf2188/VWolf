//
//  IVector2.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 8/21/24.
//

#include "vwpch.h"
#include "IVector2.h"
#include "Vector2.h"

#include <glm/gtx/perpendicular.hpp>
#include <glm/gtx/vector_angle.hpp>

namespace VWolf {
    // MARK: Constants
    const IVector2 IVector2::Up(0, 1);

    const IVector2 IVector2::Down(0, -1);

    const IVector2 IVector2::Right(1, 0);

    const IVector2 IVector2::Left(-1, 0);

    const IVector2 IVector2::Zero(0, 0);

    const IVector2 IVector2::One(1, 1);

    // MARK: Constructors
    IVector2::IVector2(): _vector2(glm::ivec2(0, 0)), x(_vector2.x), y(_vector2.y) {}

    IVector2::IVector2(int32_t x, int32_t y): _vector2(glm::ivec2(x, y)), x(_vector2.x), y(_vector2.y)  {}

    IVector2::IVector2(IVector2& vector2): _vector2(glm::ivec2(vector2._vector2.x, vector2._vector2.y)), x(_vector2.x), y(_vector2.y)  {}

    IVector2::IVector2(IVector2&& vector2): _vector2(std::move(vector2._vector2)), x(_vector2.x), y(_vector2.y)  {
        vector2._vector2.x = 0;
        vector2._vector2.y = 0;
    }

    // MARK: Destructor
    IVector2::~IVector2() {
        this->_vector2.x = 0;
        this->_vector2.y = 0;
    }

    // MARK: Assignment operators
    IVector2& IVector2::operator=(const IVector2& other) {
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
    std::ostream& operator<<(std::ostream& os, const IVector2& v) {
        os << "IVector2(" << std::addressof(v) << ") - { x: " << v.x << ", y: " << v.y << "}";
        return os;
    }

    bool IVector2::operator==(const IVector2& rhs) {
        return this->_vector2.x == rhs._vector2.x && this->_vector2.y == rhs._vector2.y;
    }

    bool operator==(const IVector2& lhs, const IVector2& rhs) {
        return lhs.x == rhs.x && lhs.y == rhs.y;
    }

    bool IVector2::operator!=(const IVector2& rhs) {
        return this->_vector2.x != rhs._vector2.x || this->_vector2.y != rhs._vector2.y;
    }

    bool operator!=(const IVector2& lhs, const IVector2& rhs) {
        return lhs.x != rhs.x || lhs.y != rhs.y;
    }

    IVector2 IVector2::operator+(const IVector2& rhs) {
        glm::vec2 vec = this->_vector2 + rhs._vector2;
        return IVector2(vec.x, vec.y);
    }

    IVector2 operator+(const IVector2& lhs, const IVector2& rhs) {
        IVector2 result = lhs + rhs;
        return result;
    }

    IVector2 IVector2::operator-(const IVector2& rhs) {
        glm::vec2 vec = this->_vector2 - rhs._vector2;
        return IVector2(vec.x, vec.y);
    }

    IVector2 operator-(const IVector2& lhs, const IVector2& rhs) {
        IVector2 result = lhs - rhs;
        return result;
    }

    IVector2 IVector2::operator*(int32_t rhs) {
        glm::vec2 vec = this->_vector2 * rhs;
        return IVector2(vec.x, vec.y);
    }

    IVector2 operator*(const IVector2& lhs, int32_t rhs) {
        IVector2 result = lhs * rhs;
        return result;
    }

    IVector2 IVector2::operator/(int32_t rhs) {
        glm::vec2 vec = this->_vector2 / rhs;
        return IVector2(vec.x, vec.y);
    }

    IVector2 operator/(const IVector2& lhs, int32_t rhs) {
        IVector2 result = lhs / rhs;
        return result;
    }

    int32_t IVector2::operator[](int index) {
        if (index < 0 || index > 1) return std::numeric_limits<int32_t>::max();

        return _vector2[index];
    }

    IVector2::operator Vector2() {
        return Vector2(this->x, this->y);
    }

    // MARK: Get Functions
    float IVector2::Magnitude() const {
        return _vector2.length();
    }

    int32_t IVector2::SqrMagnitude() const {
        return static_cast<int32_t>(glm::sqrt(Magnitude()));
    }

    // MARK: Static Functions
    IVector2 IVector2::CeilToInt(Vector2 vector) {
        return IVector2(std::ceil(vector.x), std::ceil(vector.y));
    }

    float IVector2::Distance(IVector2 a, IVector2 b) {
        float result = Vector2::Distance((Vector2)a, (Vector2)b);
        return result;
    }

    IVector2 IVector2::FloorToInt(Vector2 vector) {
        return IVector2(std::floor(vector.x), std::floor(vector.y));
    }

    IVector2 IVector2::Max(IVector2 lhs, IVector2 rhs) {
        glm::ivec2 vec2 = glm::max(lhs._vector2, rhs._vector2);
        return IVector2(vec2.x, vec2.y);
    }

    IVector2 IVector2::Min(IVector2 lhs, IVector2 rhs) {
        glm::ivec2 vec2 = glm::min(lhs._vector2, rhs._vector2);
        return IVector2(vec2.x, vec2.y);
    }

    IVector2 IVector2::RoundToInt(Vector2 vector) {
        return IVector2(std::round(vector.x), std::round(vector.y));
    }

    IVector2 IVector2::Scale(IVector2 a, IVector2 b) {
        return IVector2(a.x * b.x, a.y * b.y);
    }
}
