//
//  Light.h
//  VWolf
//
//  Created by Enrique Moises on 6/22/26.
//

#pragma once
#include "VWolf/Core/Base.h"
#include "VWolf/Core/Math/VMath.h"
#include <map>
#ifdef VWOLF_PLATFORM_MACOS
#include <vector>
#endif

#include "VWolf/Core/Utils/GenericSerialization.h"

namespace VWolf {
    class Bounds;
    struct CameraPass;

    struct Light {
    public:
        enum class LightType: unsigned int {
            Unknown = 0,
            Directional = 1,
            Spot = 2,
            Point = 3
        };
        
        Vector4 color;
        Vector4 position;
        Vector4 direction;
        Vector4 strength;
        float cutOff;
        float exponent;
        LightType type = LightType::Unknown;
        // TODO: This padding is only for OPENGL, but it could be useful for DirectX
        // TODO: In case of not being useful, pass it to the specific render.
        float padding; // Needed for OPENGL

    public:
        Matrix4x4 GetLightSpaceMatrix();
        
        VWOLF_SERIALIZATION_FRIENDS(Light)
    public:
        static const char* LightName;
        static const char* LightSpaceName;
        static const int LightsMax;
    };

    BOOST_DESCRIBE_ENUM(Light::LightType, Unknown, Directional, Spot, Point)
    BOOST_DESCRIBE_STRUCT(Light, (), (color, position, direction, strength, cutOff, exponent, type))
}

namespace YAML {
    VWOLF_CREATE_CONVERT_GENERIC_ENUM_DECODER(VWolf::Light::LightType, Unknown, Directional, Spot, Point)
    VWOLF_CREATE_CONVERT_GENERIC_CLASS_DECODER(VWolf::Light)
}
