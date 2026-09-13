//
//  Light.cpp
//  VWolf
//
//  Created by Enrique Moises on 5/16/26.
//

#include "vwpch.h"
#include "Light.h"
#include "Camera.h"

#include "VWolf/Core/Physics/Bounds.h"

namespace VWolf {
    const char* Light::LightName = "Light";

    const char* Light::LightSpaceName = "LightSpace";

    const int Light::LightsMax = 8;

    VWOLF_CREATE_CONVERT_GENERIC_ENUM_ENCODER(Light::LightType, Unknown, Directional, Spot, Point)
    VWOLF_CREATE_CONVERT_GENERIC_CLASS_ENCODER_NO_NAME(Light)

    Matrix4x4 Light::GetLightSpaceMatrix() {
        Vector3 eye(position.GetX(), position.GetY(), position.GetZ());
        Vector3 lightDir = direction;
        Vector3 up(0.0f, 1.0f, 0.0f);
        if (Mathf::Abs(Vector3::Dot(lightDir, up)) > 0.999f) {
            up = Vector3(1.0f, 0.0f, 0.0f);
        }
        Matrix4x4 lightView = Matrix4x4::LookAt(eye, eye + lightDir, up);
        Matrix4x4 lightProjection = Matrix4x4::Ortho(-30.0f, 30.0f, -30.0f, 30.0f, 1.0f, 100.0f);
        return lightProjection * lightView;
    }
}
