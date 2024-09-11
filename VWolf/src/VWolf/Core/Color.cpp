//
//  Color.cpp
//  VWolf
//
//  Created by Enrique Ricalde on 9/9/24.
//

#include "vwpch.h"
#include "Color.h"
#include "Math/Vector4.h"

#include <glm/glm.hpp>

namespace VWolf {

    // MARK: Static Expressions
    Color Color::black = Color(0, 0, 0, 1);

    Color Color::blue = Color(0, 0, 1, 1);

    Color Color::clear = Color(0, 0, 0, 0);

    Color Color::cyan = Color(0, 1, 1, 1);

    Color Color::gray = Color(0.5f, 0.5f, 0.5f, 1);

    Color Color::green = Color(0, 1, 0, 1);

    Color Color::grey = Color(0.5f, 0.5f, 0.5f, 1);

    Color Color::magenta = Color(1, 0, 1, 1);

    Color Color::red = Color(1, 0, 0, 1);

    Color Color::white = Color(1, 1, 1, 1);

    Color Color::yellow = Color(1, 0.92f, 0.016f, 1);

    // MARK: Constructors
    Color::Color(): r(0), g(0), b(0), a(0) {}

    Color::Color(float r, float g, float b, float a): r(glm::max(r, 0.0f)), g(glm::max(g, 0.0f)), b(glm::max(b, 0.0f)), a(glm::max(a, 0.0f)) {}

    Color::Color(Color& color): r(glm::max(color.r, 0.0f)), g(glm::max(color.g, 0.0f)), b(glm::max(color.b, 0.0f)), a(glm::max(color.a, 0.0f)) {}

    Color::Color(const Color& color): r(glm::max(color.r, 0.0f)), g(glm::max(color.g, 0.0f)), b(glm::max(color.b, 0.0f)), a(glm::max(color.a, 0.0f)) {}

    Color::Color(Color&& color): r(glm::max(color.r, 0.0f)), g(glm::max(color.g, 0.0f)), b(glm::max(color.b, 0.0f)), a(glm::max(color.a, 0.0f)) {
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
    bool Color::operator==(const Color& rhs) {
        return 
        this->r == rhs.r &&
        this->g == rhs.g &&
        this->b == rhs.b &&
        this->a == rhs.a;
    }

    bool Color::operator!=(const Color& rhs) {
        return !(*this == rhs);
    }

    Color Color::operator+(const Color& rhs) {
        return Color(r + rhs.r, g + rhs.g, b + rhs.b, a + rhs.a);
    }

    Color Color::operator-(const Color& rhs) {
        return Color(glm::max(r - rhs.r, 0.0f), glm::max(g - rhs.g, 0.0f), glm::max(b - rhs.b, 0.0f), glm::max(a - rhs.a, 0.0f));
    }

    Color Color::operator*(const Color& rhs) {
        return Color(r * rhs.r, g * rhs.g, b * rhs.b, a * rhs.a);
    }

    Color Color::operator/(const Color& rhs) {
        return Color(r / rhs.r, g / rhs.g, b / rhs.b, a / rhs.a);
    }

    std::ostream& operator<<(std::ostream& os, const Color& v) {
        os << "Color(" << std::addressof(v) << ") - { r: " << v.r << ", g: " << v.g << ", b: " << v.b << ", a: " << v.a << " }";
        return os;
    }

    bool operator==(const Color& lhs, const Color& rhs) {
        return
        lhs.r == rhs.r &&
        lhs.g == rhs.g &&
        lhs.b == rhs.b &&
        lhs.a == rhs.a;
    }

    bool operator!=(const Color& lhs, const Color& rhs) {
        return !(lhs == rhs);
    }

    Color operator+(const Color& lhs, const Color& rhs) {
        return Color(lhs.r + rhs.r, lhs.g + rhs.g, lhs.b + rhs.b, lhs.a + rhs.a);
    }

    Color operator-(const Color& lhs, const Color& rhs) {
        return Color(glm::max(lhs.r - rhs.r, 0.0f), glm::max(lhs.g - rhs.g, 0.0f), glm::max(lhs.b - rhs.b, 0.0f), glm::max(lhs.a - rhs.a, 0.0f));
    }

    Color operator*(const Color& lhs, const Color& rhs) {
        return Color(lhs.r * rhs.r, lhs.g * rhs.g, lhs.b * rhs.b, lhs.a * rhs.a);
    }

    Color operator/(const Color& lhs, const Color& rhs) {
        return Color(lhs.r / rhs.r, lhs.g / rhs.g, lhs.b / rhs.b, lhs.a / rhs.a);
    }

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

    // MARK: Instance Functions
    float Color::GetGrayScale() const {
        return (0.299f * r) + (0.587f * g) + (0.114f * b);
    }

    float Color::GetMaxColorComponent() const {
        return glm::max(r, glm::max(g, b));
    }

    Color Color::GetGamma() const {
        std::function<float(float)> transform = [](float value) {
            float result = 0;
            if (value <= 0.0031308f)
            {
                result = value * 12.92f;
            }
            else
            {
                // pow(rgb, 2.2f)
                result = 1.055f * std::pow(value, 1.0f / 2.4f) - 0.055f;
            }
            return result;
        };
        return Color(
                     transform(r),
                     transform(g),
                     transform(b),
                     a
                     );
    }

    Color Color::GetLinear() const {
        std::function<float(float)> transform = [](float value) {
            float result = 0;
            if (value <= 0.04045f)
            {
                result = value / 12.92f;
            }
            else
            {
                // pow(rgb, 1.0f / 2.2f)
                result = std::pow((value + 0.055f) / 1.055f, 2.4f);
            }
            return result;
        };
        return Color(
                     transform(r),
                     transform(g),
                     transform(b),
                     a
                     );
    }

    // MARK: Static Functions
    Color Color::Lerp(Color a, Color b, float t) {
        Vector4 aa = (Vector4)a;
        Vector4 bb = (Vector4)b;
        return (Color)Vector4::Lerp(aa, bb, t);
    }

    Color Color::LerpUnclamped(Color a, Color b, float t) {
        Vector4 aa = (Vector4)a;
        Vector4 bb = (Vector4)b;
        return (Color)Vector4::LerpUnclamped(aa, bb, t);
    }

    Color Color::HSVToRGB(float H, float S, float V) {
        float fC = V * S; // Chroma
        float fHPrime = std::fmod(H / 60.0, 6);
        float fX = fC * (1 - std::fabs(std::fmod(fHPrime, 2) - 1));
        float fM = V - fC;

        float fR = 0, fG = 0, fB = 0;

        if (0 <= fHPrime && fHPrime < 1) {
            fR = fC;
            fG = fX;
            fB = 0;
        } else if (1 <= fHPrime && fHPrime < 2) {
            fR = fX;
            fG = fC;
            fB = 0;
        } else if (2 <= fHPrime && fHPrime < 3) {
            fR = 0;
            fG = fC;
            fB = fX;
        } else if (3 <= fHPrime && fHPrime < 4) {
            fR = 0;
            fG = fX;
            fB = fC;
        } else if (4 <= fHPrime && fHPrime < 5) {
            fR = fX;
            fG = 0;
            fB = fC;
        } else if (5 <= fHPrime && fHPrime < 6) {
            fR = fC;
            fG = 0;
            fB = fX;
        } else {
            fR = 0;
            fG = 0;
            fB = 0;
        }

        fR += fM;
        fG += fM;
        fB += fM;

        return Color(fR, fG, fB, 1);
    }

//    Color Color::HSVToRGB(float H, float S, float V, bool hdr) {
//
//    }

    void Color::RGBToHSV(Color rgbColor, float& H, float& S, float& V) {
        float fCMax = glm::max(glm::max(rgbColor.r, rgbColor.g), rgbColor.b);
        float fCMin = glm::min(glm::min(rgbColor.r, rgbColor.g), rgbColor.b);
        float fDelta = fCMax - fCMin;

        if (fDelta > 0) {
            if (fCMax == rgbColor.r) {
                H = 60 * (fmod(((rgbColor.g - rgbColor.b) / fDelta), 6));
            } else if (fCMax == rgbColor.g) {
                H = 60 * (((rgbColor.b - rgbColor.r) / fDelta) + 2);
            } else if (fCMax == rgbColor.b) {
                H = 60 * (((rgbColor.r - rgbColor.g) / fDelta) + 4);
            }

            if (fCMax > 0) {
                S = fDelta / fCMax;
            } else {
                S = 0;
            }

            V = fCMax;
        } else {
            H = 0;
            S = 0;
            V = fCMax;
        }

        if (H < 0) {
            H = 360 + H;
        }
    }
}
