//
//  Color.hpp
//  VWolf
//
//  Created by Enrique Ricalde on 9/9/24.
//

#pragma once
#include "VWolf/Core/Base.h"

// TODO: Incomplete
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
    private:
        float r;
        float g;
        float b;
        float a;
    };

}
