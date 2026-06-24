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
            // calculateShadowFrustumBox
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

/*
 cpp#include <glm/glm.hpp>
 #include <glm/gtc/matrix_transform.hpp>
 #include <glm/gtc/type_ptr.hpp>
 #include <limits>

 struct AABB {
     glm::vec3 min;
     glm::vec3 max;
 };

 // Calculates the light's orthographic projection for a given camera frustum and light direction
 AABB calculateShadowFrustumBox(const glm::mat4& viewMatrix,
                                const glm::mat4& projMatrix,
                                const glm::vec3& lightDir) {
     
     // 1. Get the 8 corners of the view frustum in NDC (Normalized Device Coordinates)
     // NDC is a 2x2x2 cube ranging from -1 to 1 in all dimensions.
     glm::vec4 ndcCorners[8] = {
         { -1.0f, -1.0f, -1.0f, 1.0f },
         {  1.0f, -1.0f, -1.0f, 1.0f },
         {  1.0f,  1.0f, -1.0f, 1.0f },
         { -1.0f,  1.0f, -1.0f, 1.0f },
         { -1.0f, -1.0f,  1.0f, 1.0f },
         {  1.0f, -1.0f,  1.0f, 1.0f },
         {  1.0f,  1.0f,  1.0f, 1.0f },
         { -1.0f,  1.0f,  1.0f, 1.0f }
     };

     // 2. Calculate the inverse of the View-Projection matrix to go from NDC to World Space
     glm::mat4 viewProj = projMatrix * viewMatrix;
     glm::mat4 invViewProj = glm::inverse(viewProj);

     glm::vec4 worldCorners[8];
     float minX = std::numeric_limits<float>::max();
     float maxX = std::numeric_limits<float>::lowest();
     float minY = std::numeric_limits<float>::max();
     float maxY = std::numeric_limits<float>::lowest();
     float minZ = std::numeric_limits<float>::max();
     float maxZ = std::numeric_limits<float>::lowest();

     // 3. Transform NDC corners to World Space
     for (int i = 0; i < 8; ++i) {
         worldCorners[i] = invViewProj * ndcCorners[i];
         worldCorners[i] /= worldCorners[i].w; // Perform perspective divide
     }

     // 4. Create the Light View Matrix
     // The direction vector represents where the light is looking.
     // For shadows, a standard approach is centering the light view slightly behind the frustum center.
     glm::vec3 frustumCenter = glm::vec3(0.0f);
     for (int i = 0; i < 8; ++i) {
         frustumCenter += glm::vec3(worldCorners[i]);
     }
     frustumCenter /= 8.0f;

     // Define a stable up vector (assuming +Y is up)
     glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
     
     // Handle edge case where light direction points straight up or down
     if (glm::abs(glm::dot(lightDir, up)) > 0.999f) {
         up = glm::vec3(1.0f, 0.0f, 0.0f);
     }

     glm::mat4 lightView = glm::lookAt(frustumCenter - lightDir, frustumCenter, up);

     // 5. Transform the 8 world space corners into Light View Space
     // and find the minimum and maximum coordinates.
     for (int i = 0; i < 8; ++i) {
         glm::vec4 lightSpaceCorner = lightView * worldCorners[i];
         
         minX = glm::min(minX, lightSpaceCorner.x);
         maxX = glm::max(maxX, lightSpaceCorner.x);
         minY = glm::min(minY, lightSpaceCorner.y);
         maxY = glm::max(maxY, lightSpaceCorner.y);
         minZ = glm::min(minZ, lightSpaceCorner.z);
         maxZ = glm::max(maxZ, lightSpaceCorner.z);
     }

     // Return the light-space AABB used to create the Orthographic Projection
     return { glm::vec3(minX, minY, minZ), glm::vec3(maxX, maxY, maxZ) };
 }
 */
