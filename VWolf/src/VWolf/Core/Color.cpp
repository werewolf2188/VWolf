//
//  Color.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 9/9/24.
//

#include "vwpch.h"
#include "Color.h"
#include "Math/Vector4.h"

namespace VWolf {
    // MARK: Constructors
    Color::Color(): r(0), g(0), b(0), a(0) {}

    Color::Color(float r, float g, float b, float a): r(r), g(g), b(b), a(a) {}

    Color::Color(Color& color): r(color.r), g(color.g), b(color.b), a(color.a) {}

    Color::Color(const Color& color):  r(color.r), g(color.g), b(color.b), a(color.a) {}

    Color::Color(Color&& color): r(color.r), g(color.g), b(color.b), a(color.a) {
        color.r = 0;
        color.g = 0;
        color.b = 0;
        color.a = 0;
    }

    // MARK: Destructor
    Color::~Color() {
        this->r = 0;
        this->g = 0;
        this->b = 0;
        this->a = 0;
    }

    // MARK: Assignment operators
    Color& Color::operator=(const Color& other) {
        this->r = other.r;
        this->g = other.g;
        this->b = other.b;
        this->a = other.a;

        return *this;
    }

    // MARK: Operator overloading
    float Color::operator[](int index) {
        if (index < 0 || index > 3) return std::numeric_limits<float>::max();

        switch (index) {
            case 0: return r;
            case 1: return g;
            case 2: return b;
            case 3: return a;
        }
        throw std::exception();
    }

    Color::operator Vector4() {
        return Vector4(r, g, b, a);
    }

    // MARK: Get Functions
    const float Color::GetR() const { return r; }

    float& Color::GetR() { return r; }

    const float Color::GetG() const { return g; }

    float& Color::GetG() { return g; }

    const float Color::GetB() const { return b; }

    float& Color::GetB() { return b; }

    const float Color::GetA() const { return a; }

    float& Color::GetA() { return a; }

    void Color::SetR(float value) { r = value; }

    void Color::SetG(float value) { g = value; }

    void Color::SetB(float value) { b = value; }

    void Color::SetA(float value) { a = value; }
}
