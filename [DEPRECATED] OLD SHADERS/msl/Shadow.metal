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
    float4x4 u_ViewProjection;
};

VertexPayload vertex vertexMain(
//                                uint vertexID [[vertex_id]],
                                VertexIn vertexIn [[stage_in]],
                                constant Object &object [[buffer(1)]],
                                constant Camera &camera [[buffer(2)]]
                                ) {
    VertexPayload payload;

    payload.position = camera.u_ViewProjection * object.u_Transform * float4(vertexIn.position, 1.0);
    payload.position.z = payload.position.z * 0.5 + 0.5;

    return payload;
}

float4 fragment fragmentMain(VertexPayload frag [[stage_in]]) {
    float4 diffuseAlbedo = float4(1,0,0,1);
    return diffuseAlbedo;
}
