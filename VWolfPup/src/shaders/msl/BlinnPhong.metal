//
// test.metal
// VWolfPup
//
// Created by Enrique Ricalde on 4/25/2024
//

#include <metal_stdlib>
using namespace metal;

#define LIGHTS_MAX 8

struct VertexIn
{
    float3 position [[attribute(0)]];
    float4 color [[attribute(1)]];
    float3 normal [[attribute(2)]];
    float3 tangent [[attribute(3)]];
    float2 texCoord [[attribute(4)]];
};

struct VertexPayload {
    float4 position [[position]];
    float3 positionL;
    float3 normal;
    float2 texCoord;
    float4 FragPosLightSpace0;
    float4 FragPosLightSpace1;
    float4 FragPosLightSpace2;
    float4 FragPosLightSpace3;
    float4 FragPosLightSpace4;
    float4 FragPosLightSpace5;
    float4 FragPosLightSpace6;
    float4 FragPosLightSpace7;
//    float4 FragPosLightSpace[LIGHTS_MAX];
};

#define setFragPosLightSpace(object, index) payload.FragPosLightSpace##index

struct Object {
    float4x4 u_Transform;
};

struct Camera
{
    /*
        TODO: I don't know what this value does
         float cbPerObjectPad1;
     */
    float4x4 u_View;
    float4x4 u_InvView;
    float4x4 u_Proj;
    float4x4 u_InvProj;
    float4x4 u_ViewProjection;
    float4x4 u_InvViewProjection;
    float3 u_EyePosition;
    float2 u_RenderTargetSize;
    float2 u_InvRenderTargetSize;
    float u_NearZ;
    float u_FarZ;
    float u_TotalTime;
    float u_DeltaTime;
};

struct Material {
    float4 u_ambientColor;
    float4 u_diffuseColor;
    float3 u_specular;
    float u_shinines;
};

struct LightInfo {
    float4 u_color;
    float4 u_position;
    float4 u_direction;
    float4 u_strength;
    float u_cutOff;
    float u_exponent;
    uint u_type;
};

struct PerLight {
    LightInfo light[LIGHTS_MAX];
};

struct LightSpaceInfo {
    float4x4 u_lightSpaceMatrix;
};

struct PerLightSpace {
    LightSpaceInfo lightSpaces[LIGHTS_MAX];
};

constexpr sampler linearSampler(coord::normalized, min_filter::linear, mag_filter::linear, mip_filter::linear);

inline float3x3 tofloat3x3(float4x4 m) {
    return float3x3(m[0].xyz, m[1].xyz, m[2].xyz);
}
inline half3x3 tohalf3x3(half4x4 m) {
    return half3x3(m[0].xyz, m[1].xyz, m[2].xyz);
}

float4 returnFragPosLightSpace(VertexPayload frag, int index) {
    switch (index) {
        case 0: return frag.FragPosLightSpace0;
        case 1: return frag.FragPosLightSpace1;
        case 2: return frag.FragPosLightSpace2;
        case 3: return frag.FragPosLightSpace3;
        case 4: return frag.FragPosLightSpace4;
        case 5: return frag.FragPosLightSpace5;
        case 6: return frag.FragPosLightSpace6;
        case 7: return frag.FragPosLightSpace7;
    }
    return float4(0, 0, 0, 0);
}

float ShadowCalculation(float4 fragPosLightSpace, texture2d<float, access::sample> Shadow) {
    float3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords.x = projCoords.x * 0.5 + 0.5;
    projCoords.y = projCoords.y * (-0.5) + 0.5;
    float closestDepth = Shadow.sample(linearSampler, projCoords.xy).r;//texture(u_shadow, projCoords.xy).r;
    float currentDepth = (projCoords.z * 0.5 + 0.5);
    float bias = 0.005;
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    if ((projCoords.z * 0.5 + 0.5) > 1.0)
        shadow = 0.0;

    return shadow;
}

half3 ComputeDirectionalBlinnPhongLightColor(float3 position, float3 n,
                                              constant Material &material, LightInfo lightInfo, float4 FragPosLightSpace,
                                              texture2d<float, access::sample> Shadow) {

    // Ambient
    float3 ambient = (material.u_ambientColor * lightInfo.u_color).xyz;

    // Diffuse
    float3 s = float3(0, 0, 0);
    s = normalize(lightInfo.u_direction.xyz);
    float sDotN = max(dot(s, n), 0.0);
    float3 diffuse = (lightInfo.u_color * material.u_diffuseColor * sDotN).xyz;

    // Specular
    float3 spec = float3(0.0, 0.0, 0.0);
    if (sDotN > 0) {
        float3 v = normalize(-position.xyz);
        float3 h = normalize(v + s);
        spec = material.u_specular * pow(max(dot(h, n), 0.0), material.u_shinines);
    }
    float shadow = ShadowCalculation(FragPosLightSpace, Shadow);
    float3 color = ambient + (1.0 - shadow) * lightInfo.u_strength.xyz * (diffuse + spec);
    return half3(color.x, color.y, color.z);
}

half3 ComputePointBlinnPhongLightColor(float3 position, float3 n,
                                       float4x4 u_View,
                                       constant Material &material, LightInfo lightInfo, float4 FragPosLightSpace,
                                       texture2d<float, access::sample> Shadow) {
    // Light position
    float4 LightPosition = u_View * lightInfo.u_position;

    // Ambient
    float3 ambient = (material.u_ambientColor * lightInfo.u_color).xyz;

    // Diffuse
    float3 s = float3(0, 0, 0);
    s = normalize(LightPosition.xyz - position);
    float sDotN = max(dot(s, n), 0.0);
    float3 diffuse = (lightInfo.u_color * material.u_diffuseColor * sDotN).xyz;

    // Specular
    float3 spec = float3(0.0, 0.0, 0.0);
    if (sDotN > 0) {
        float3 v = normalize(-position.xyz);
        float3 h = normalize(v + s);
        spec = material.u_specular * pow(max(dot(h, n), 0.0), material.u_shinines);
    }
    float shadow = ShadowCalculation(FragPosLightSpace, Shadow);
    float3 color = ambient + (1.0 - shadow) * lightInfo.u_strength.xyz * (diffuse + spec);
    return half3(color.x, color.y, color.z);
}

half3 ComputeSpotBlinnPhongLightColor(float3 position, float3 n, 
                                       float4x4 u_View,
                                       constant Material &material, LightInfo lightInfo, float4 FragPosLightSpace,
                                       texture2d<float, access::sample> Shadow) {
    
    // Light position
    float4 LightPosition = u_View * lightInfo.u_position;

    // Light direction
    float4 LightDirection = u_View * lightInfo.u_direction;

    // Ambient
    float3 ambient = (material.u_ambientColor * lightInfo.u_color).xyz;

    
    float3 s = float3(0, 0, 0);
    s = normalize(LightPosition.xyz - position);
    float cosAng = dot(-s, normalize(LightDirection.xyz));
    float angle = acos(cosAng);
    float spotScale = 0.0;

    // Diffuse
    float3 diffuse = float3(0.0, 0.0, 0.0);

    // Spec
    float3 spec = float3(0.0, 0.0, 0.0);

    if (angle >= 0.0 && angle < lightInfo.u_cutOff) {
        spotScale = pow(cosAng, lightInfo.u_exponent);
        float sDotN = max(dot(s, n), 0.0);
        diffuse = (lightInfo.u_color * material.u_diffuseColor * sDotN).xyz;

        // Specular
        if (sDotN > 0) {
            float3 v = normalize(-position.xyz);
            float3 h = normalize(v + s);
            spec = material.u_specular * pow(max(dot(h, n), 0.0), material.u_shinines);
        }
    }
    float shadow = ShadowCalculation(FragPosLightSpace, Shadow);
    float3 color = ambient + (1.0 - shadow) * spotScale * lightInfo.u_strength.xyz * (diffuse + spec);
    return half3(color.x, color.y, color.z);
}

half3 ComputeBlinnPhongLightColor(VertexPayload frag, constant Camera &camera, constant Material &material, constant PerLight &light, texture2d<float, access::sample> Shadow) {
    half3 color = half3(0.0, 0.0, 0.0);

    float3 n = normalize(frag.normal);
    float3 position = frag.positionL;
    for (int i = 0; i < LIGHTS_MAX; i++) {
        float4 FragPosLightSpace = returnFragPosLightSpace(frag, i);
        if (light.light[i].u_type == 1) { // Directional
            color += ComputeDirectionalBlinnPhongLightColor(position, n, material, light.light[i], FragPosLightSpace, Shadow);
        }
        else if (light.light[i].u_type == 2) { // Spotlight
            color += ComputeSpotBlinnPhongLightColor(position, n, camera.u_View, material, light.light[i], FragPosLightSpace, Shadow);
        }
        else if (light.light[i].u_type == 3) { // Point
            color += ComputePointBlinnPhongLightColor(position, n, camera.u_View, material, light.light[i], FragPosLightSpace, Shadow);
        }
    }
    return color;
}

VertexPayload vertex vertexMain(
//                                uint vertexID [[vertex_id]],
                                VertexIn vertexIn [[stage_in]],
                                constant Object &object [[buffer(1)]],
                                constant Camera &camera [[buffer(2)]],
                                constant PerLightSpace& lightspace [[buffer(3)]]
                                ) {
    VertexPayload payload;

    payload.position = camera.u_ViewProjection * object.u_Transform * float4(vertexIn.position, 1.0);
    float3x3 normalMatrix = tofloat3x3(camera.u_View * object.u_Transform);
    payload.positionL = (camera.u_View * object.u_Transform * float4(vertexIn.position, 1.0)).xyz;
    payload.normal = normalize(normalMatrix * vertexIn.normal);
    payload.texCoord = vertexIn.texCoord;

    float4 posT = object.u_Transform * float4(vertexIn.position, 1.0);
    // Unrolling
    setFragPosLightSpace(payload, 0) = lightspace.lightSpaces[0].u_lightSpaceMatrix * posT;
    setFragPosLightSpace(payload, 1) = lightspace.lightSpaces[1].u_lightSpaceMatrix * posT;
    setFragPosLightSpace(payload, 2) = lightspace.lightSpaces[2].u_lightSpaceMatrix * posT;
    setFragPosLightSpace(payload, 3) = lightspace.lightSpaces[3].u_lightSpaceMatrix * posT;
    setFragPosLightSpace(payload, 4) = lightspace.lightSpaces[4].u_lightSpaceMatrix * posT;
    setFragPosLightSpace(payload, 5) = lightspace.lightSpaces[5].u_lightSpaceMatrix * posT;
    setFragPosLightSpace(payload, 6) = lightspace.lightSpaces[6].u_lightSpaceMatrix * posT;
    setFragPosLightSpace(payload, 7) = lightspace.lightSpaces[7].u_lightSpaceMatrix * posT;

    return payload;
}

half4 fragment fragmentMain(VertexPayload frag [[stage_in]],
                            constant Material &material [[buffer(1)]],
                            constant Camera &camera [[buffer(2)]],
                            constant PerLight &light [[buffer(3)]],
                            texture2d<float, access::sample> diffuseTexture [[texture(0)]],
                            texture2d<float, access::sample> Shadow [[texture(1)]]//,
                            /*sampler samplr [[sampler(0)]])*/) {

    float4 sampleTexture = diffuseTexture.sample(linearSampler, float2(1.0f - frag.texCoord.x,frag.texCoord.y));
    return half4(sampleTexture) * half4(ComputeBlinnPhongLightColor(frag, camera, material, light, Shadow), 1.0);
}
