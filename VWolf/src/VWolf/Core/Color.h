//
//  Color.hpp
//  VWolf
//
//  Created by Enrique Ricalde on 9/9/24.
//

#pragma once
#include "VWolf/Core/Base.h"

namespace VWolf {
    struct Vector4;

    struct Color {
    public:
        Color();
        Color(float, float, float, float);
        Color(Color &);
        Color(const Color &);
        Color(Color &&);
        ~Color();
    public:
        Color& operator=(const Color& other);
//        Vector2& operator=(Vector2&& other);
    public:
        static Color black;
        static Color blue;
        static Color clear;
        static Color cyan;
        static Color gray;
        static Color green;
        static Color grey;
        static Color magenta;
        static Color red;
        static Color white;
        static Color yellow;
    public:
        bool operator==(const Color& rhs);
        bool operator!=(const Color& rhs);
        Color operator+(const Color& rhs);
        Color operator-(const Color& rhs);
        Color operator*(const Color& rhs);
        Color operator/(const Color& rhs);
        float operator[](int index);
        operator Vector4();
    public:
        const float GetR() const;
        float& GetR();
        const float GetG() const;
        float& GetG();
        const float GetB() const;
        float& GetB();
        const float GetA() const;
        float& GetA();
        void SetR(float value);
        void SetG(float value);
        void SetB(float value);
        void SetA(float value);
    public:
        float GetGrayScale() const;
        float GetMaxColorComponent() const;
        Color GetGamma() const;
        Color GetLinear() const;
    public:
        static Color Lerp(Color a, Color b, float t);
        static Color LerpUnclamped(Color a, Color b, float t);
        static Color HSVToRGB(float H, float S, float V);
//        static Color HSVToRGB(float H, float S, float V, bool hdr);
        static void RGBToHSV(Color rgbColor, float& H, float& S, float& V);
    private:
        float r;
        float g;
        float b;
        float a;
    public:
        friend std::ostream& operator<<(std::ostream& os, const Color& v);
        friend bool operator==(const Color& lhs, const Color& rhs);
        friend bool operator!=(const Color& lhs, const Color& rhs);
        friend Color operator+(const Color& lhs, const Color& rhs);
        friend Color operator-(const Color& lhs, const Color& rhs);
        friend Color operator*(const Color& lhs, const Color& rhs);
        friend Color operator/(const Color& lhs, const Color& rhs);
    };

}
