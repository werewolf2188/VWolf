//
//  Texture.hpp
//  VWolf
//
//  Created by Enrique Ricalde on 1/25/23.
//

#pragma once
#include "VWolf/Core/Base.h"
namespace VWolf {
    class Texture2D;

    class Texture {
    public:
        virtual ~Texture() = default;
    public:
        static Ref<Texture2D> LoadTexture2D(uint32_t width, uint32_t height);
    };

    class Texture2D: public Texture {
    public:
        Texture2D(uint32_t width, uint32_t height): m_width(width), m_height(height) {}
        virtual ~Texture2D() = default;
    public:
        virtual void PopulateTest() = 0;
    protected:
        uint32_t m_width;
        uint32_t m_height;
    };
}

