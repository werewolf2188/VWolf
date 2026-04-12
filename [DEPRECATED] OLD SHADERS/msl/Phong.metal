//
// test.metal
// VWolfPup
//
// Created by Enrique Ricalde on 4/25/2024
//

#include <metal_stdlib>
using namespace metal;

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
    float4 color;
    float2 texCoord;
};

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

#define LIGHTS_MAX 8

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

inline float3x3 tofloat3x3(float4x4 m) {
    return float3x3(m[0].xyz, m[1].xyz, m[2].xyz);
}
inline half3x3 tohalf3x3(half4x4 m) {
    return half3x3(m[0].xyz, m[1].xyz, m[2].xyz);
}

float4 ComputePhongLightColor(VertexIn vin, 
                              constant Object &object,
                              constant Camera &camera,
                              constant Material &material,
                              constant PerLight &light) {
    float3x3 normalMatrix = tofloat3x3(camera.u_View * object.u_Transform);

    float3 n = normalize(normalMatrix * vin.normal);

    float4 camCoords = camera.u_View * object.u_Transform * float4(vin.position, 1.0f);

    float4 ambient = material.u_ambientColor * light.light[0].u_color;
    float4 lightPosition = camera.u_View * light.light[0].u_position;

    float3 s = normalize((lightPosition - camCoords).xyz);
    float sDotN = max(dot(s, n), 0.0);
    float4 diffuse = light.light[0].u_color * material.u_diffuseColor * sDotN;

    float3 spec = float3(0.0, 0.0, 0.0);
    if (sDotN > 0.0) {
        float3 v = normalize(-camCoords.xyz);
        float3 r = reflect(-s, n);
        float maximum = max(dot(r, v), 0.0);
        float shiny = pow(maximum, material.u_shinines);
        spec = light.light[0].u_color.xyz * material.u_specular * shiny;
    }
    return ambient + diffuse + float4(spec, 1.0);
}

VertexPayload vertex vertexMain(
//                                uint vertexID [[vertex_id]],
                                VertexIn vertexIn [[stage_in]],
                                constant Object &object [[buffer(1)]],
                                constant Camera &camera [[buffer(2)]],
                                constant PerLightSpace& lightspace [[buffer(3)]],
                                constant Material &material [[buffer(4)]],
                                constant PerLight &light [[buffer(5)]]
                                ) {
    VertexPayload payload;

    payload.position = camera.u_ViewProjection * object.u_Transform * float4(vertexIn.position, 1.0);
    payload.color = ComputePhongLightColor(vertexIn, object, camera, material, light);
    payload.texCoord = vertexIn.texCoord;

    return payload;
}



half4 fragment fragmentMain(VertexPayload frag [[stage_in]]) {
    return half4(frag.color.x, frag.color.y, frag.color.z, 1.0);
}
