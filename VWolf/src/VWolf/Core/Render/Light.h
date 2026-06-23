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
    // Lights
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
        LightType type = LightType::Directional;
        // TODO: This padding is only for OPENGL, but it could be useful for DirectX
        // TODO: In case of not being useful, pass it to the specific render.
        float padding; // Needed for OPENGL

    public:
        Matrix4x4 GetLightSpaceMatrix() {
            Vector3 rotation(direction.GetX(), direction.GetY(), direction.GetZ());
            Vector3 eye(position.GetX(), position.GetY(), position.GetZ());
            Quaternion q = static_cast<Quaternion>(rotation.Orientate());
            Vector3 up(q.GetOrientation({ 0.0f, 1.0f, 0.0f }));
            Vector3 center(q.GetOrientation({ 0.0f, 0.0f, -1.0f }));
            Matrix4x4 lightView = Matrix4x4::LookAt(eye, center, up);
            float near_plane = 1.0f, far_plane = 100.0f;
            Matrix4x4 lightProjection = Matrix4x4::Ortho(-30.0f, 30.0f, -30.0f, 30.0f, near_plane, far_plane);

            /*
            float near_plane = 1, far_plane = 25.0f;
            MatrixFloat4x4 lightProjection = ortho(-30.0f, 30.0f, 30.0f, -30.0f, near_plane, far_plane);
            */

            return lightProjection * lightView;
        }
        
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
