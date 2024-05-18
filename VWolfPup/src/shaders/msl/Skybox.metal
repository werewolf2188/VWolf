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
    float3 texCoord;
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

VertexPayload vertex vertexMain(
//                                uint vertexID [[vertex_id]],
                                VertexIn vertexIn [[stage_in]],
                                constant Object &object [[buffer(1)]],
                                constant Camera &camera [[buffer(2)]],
                                constant PerLightSpace& lightspace [[buffer(3)]]
                                ) {
    VertexPayload payload;

    float4 pos = camera.u_View * float4(vertexIn.position, 1.0f);
    pos = camera.u_Proj * pos;
    payload.position = float4(pos.x, pos.y, pos.w, pos.w);
    
    payload.texCoord = float3(vertexIn.position.x, vertexIn.position.y, -vertexIn.position.z);
    return payload;
}

half4 fragment fragmentMain(VertexPayload frag [[stage_in]],
                            constant Material &material [[buffer(1)]],
                            constant PerLight &light [[buffer(2)]],
                            texturecube<float, access::sample> skyTexture [[texture(0)]]//,
                            /*sampler samplr [[sampler(0)]])*/) {
    constexpr sampler linearSampler(coord::normalized, min_filter::linear, mag_filter::linear, mip_filter::linear);
    float4 color = skyTexture.sample(linearSampler, frag.texCoord);

    return half4(color.x, color.y, color.z, 1.0);
}
