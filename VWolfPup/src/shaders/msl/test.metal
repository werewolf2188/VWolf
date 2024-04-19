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


VertexPayload vertex vertexMain(
//                                uint vertexID [[vertex_id]],
                                VertexIn vertexIn [[stage_in]],
                                constant Object &object [[buffer(1)]],
                                constant Camera &camera [[buffer(2)]]
                                ) {
    VertexPayload payload;

    payload.position = camera.u_ViewProjection * object.u_Transform * float4(vertexIn.position, 1.0);
    payload.color = vertexIn.color;

    return payload;
}

half4 fragment fragmentMain(VertexPayload frag [[stage_in]]) {
    return half4(frag.color);
}
