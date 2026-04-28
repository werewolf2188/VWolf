#include "Camera.hlsli"
#include "Object.hlsli"
#include "Material.hlsli"

float4 VWolfTransformObjectSpaceToHClip(float3 positionOS) {
    return VWolfTransformWorldSpaceToHClip(mul(u_Transform, float4(positionOS, 1.0f)));
}
