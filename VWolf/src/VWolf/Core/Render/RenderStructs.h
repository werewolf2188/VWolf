//
//  VertexInfo.h
//  VWolf
//
//  Created by Enrique Ricalde on 10/11/22.
//

#pragma once
#include "VWolf/Core/Base.h"
#include "VWolf/Core/Math/VMath.h"
#include <map>
#ifdef VWOLF_PLATFORM_MACOS
#include <vector>
#endif

#include "VWolf/Core/Utils/GenericSerialization.h"

/*
 To get the bounds from a mesh, you typically calculate an Axis-Aligned Bounding Box (AABB) by finding the minimum and maximum coordinates of all vertices along the X, Y, and Z axes.General Algorithm (Pseudocode)Initialize minX, minY, minZ to positive infinity.Initialize maxX, maxY, maxZ to negative infinity.For each vertex in the mesh:Update the min values if the current vertex's coordinates are smaller.Update the max values if the current vertex's coordinates are larger.The final min and max points define the corners of your bounding box.
 */

namespace VWolf {
    enum class ShaderDataType
    {
        None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
    };

    static uint32_t ShaderDataTypeSize(ShaderDataType type)
    {
        switch (type)
        {
        case ShaderDataType::Float:    return 4;
        case ShaderDataType::Float2:   return 4 * 2;
        case ShaderDataType::Float3:   return 4 * 3;
        case ShaderDataType::Float4:   return 4 * 4;
        case ShaderDataType::Mat3:     return 4 * 3 * 3;
        case ShaderDataType::Mat4:     return 4 * 4 * 4;
        case ShaderDataType::Int:      return 4;
        case ShaderDataType::Int2:     return 4 * 2;
        case ShaderDataType::Int3:     return 4 * 3;
        case ShaderDataType::Int4:     return 4 * 4;
        case ShaderDataType::Bool:     return 1;
        case ShaderDataType::None:     return 0;
        }

        //VWOLF_CORE_ASSERT(false, "Unknown ShaderDataType!"); // ??????
        return 0;
    }

    // Camera Pass
    struct CameraPass {
        /* TODO: I don't know what this value does
             float cbPerObjectPad1;
         */
        Matrix4x4 view;
        Matrix4x4 invView;
        Matrix4x4 proj;
        Matrix4x4 invProj;
        Matrix4x4 viewProjection;
        Matrix4x4 invViewProjection;
        Vector3 eyePosition;
        float padding;
        Vector2 renderTargetSize;
        Vector2 invRenderTargetSize;
        float nearZ;
        float farZ;
        float totalTime;
        float deltaTime;
    };

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

