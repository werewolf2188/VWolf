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
    IVector2::IVector2(): _vector2(glm::ivec2(0, 0)) {}

    IVector2::IVector2(int32_t x, int32_t y): _vector2(glm::ivec2(x, y)) {}

    IVector2::IVector2(IVector2& vector2): _vector2(glm::ivec2(vector2._vector2.x, vector2._vector2.y)) {}

    IVector2::IVector2(const IVector2& vector2): _vector2(glm::ivec2(vector2._vector2.x, vector2._vector2.y)) {}

    IVector2::IVector2(IVector2&& vector2): _vector2(std::move(vector2._vector2)) {
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
        os << "IVector2(" << std::addressof(v) << ") - { x: " << v.GetX() << ", y: " << v.GetY() << "}";
        return os;
    }

    bool IVector2::operator==(const IVector2& rhs) {
        return this->_vector2.x == rhs._vector2.x && this->_vector2.y == rhs._vector2.y;
    }

    bool operator==(const IVector2& lhs, const IVector2& rhs) {
        return lhs.GetX() == rhs.GetX() && lhs.GetY() == rhs.GetY();
    }

    bool IVector2::operator!=(const IVector2& rhs) {
        return this->_vector2.x != rhs._vector2.x || this->_vector2.y != rhs._vector2.y;
    }

    bool operator!=(const IVector2& lhs, const IVector2& rhs) {
        return lhs.GetX() != rhs.GetX() || lhs.GetY() != rhs.GetY();
    }

    IVector2 IVector2::operator+(const IVector2& rhs) {
        glm::ivec2 vec = this->_vector2 + rhs._vector2;
        return IVector2(vec.x, vec.y);
    }

    IVector2 operator+(const IVector2& lhs, const IVector2& rhs) {
        glm::ivec2 vec = lhs._vector2 + rhs._vector2;
        return IVector2(vec.x, vec.y);
    }

    IVector2 IVector2::operator-(const IVector2& rhs) {
        glm::ivec2 vec = this->_vector2 - rhs._vector2;
        return IVector2(vec.x, vec.y);
    }

    IVector2 operator-(const IVector2& lhs, const IVector2& rhs) {
        glm::ivec2 vec = lhs._vector2 - rhs._vector2;
        return IVector2(vec.x, vec.y);
    }

    IVector2 IVector2::operator*(int32_t rhs) {
        glm::ivec2 vec = this->_vector2 * rhs;
        return IVector2(vec.x, vec.y);
    }

    IVector2 operator*(const IVector2& lhs, int32_t rhs) {
        glm::ivec2 vec = lhs._vector2 * rhs;
        return IVector2(vec.x, vec.y);
    }

    IVector2 operator*(int32_t lhs, const IVector2& rhs) {
        glm::ivec2 vec = lhs * rhs._vector2;
        return IVector2(vec.x, vec.y);
    }

    IVector2 IVector2::operator/(int32_t rhs) {
        glm::ivec2 vec = this->_vector2 / rhs;
        return IVector2(vec.x, vec.y);
    }

    IVector2 operator/(const IVector2& lhs, int32_t rhs) {
        glm::ivec2 vec = lhs._vector2 / rhs;
        return IVector2(vec.x, vec.y);
    }

    IVector2 operator/(int32_t lhs, const IVector2& rhs) {
        glm::ivec2 vec = lhs / rhs._vector2;
        return IVector2(vec.x, vec.y);
    }

    int32_t IVector2::operator[](int index) {
        if (index < 0 || index > 1) return std::numeric_limits<int32_t>::max();

        return _vector2[index];
    }

    IVector2::operator Vector2() {
        return Vector2(this->_vector2.x, this->_vector2.y);
    }

    // MARK: Get Functions
    const int32_t IVector2::GetX() const { return _vector2.x; }

    int32_t& IVector2::GetX() { return _vector2.x; }

    const int32_t IVector2::GetY() const { return _vector2.y; }

    int32_t& IVector2::GetY() { return _vector2.y; }

    void IVector2::SetX(int32_t value) { _vector2.x = value; }

    void IVector2::SetY(int32_t value) { _vector2.y = value; }

    float IVector2::Magnitude() const {
        return _vector2.length();
    }

    int32_t IVector2::SqrMagnitude() const {
        return static_cast<int32_t>(glm::sqrt(Magnitude()));
    }

    // MARK: Static Functions
    IVector2 IVector2::CeilToInt(Vector2 vector) {
        return IVector2(std::ceil(vector.GetX()), std::ceil(vector.GetY()));
    }

    float IVector2::Distance(IVector2 a, IVector2 b) {
        float result = Vector2::Distance((Vector2)a, (Vector2)b);
        return result;
    }

    IVector2 IVector2::FloorToInt(Vector2 vector) {
        return IVector2(std::floor(vector.GetX()), std::floor(vector.GetY()));
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
        return IVector2(std::round(vector.GetX()), std::round(vector.GetY()));
    }

    IVector2 IVector2::Scale(IVector2 a, IVector2 b) {
        return IVector2(a._vector2.x * b._vector2.x, a._vector2.y * b._vector2.y);
    }
}
