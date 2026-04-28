cbuffer cbPerCamera : register(b0)
{
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


float4 VWolfTransformWorldSpaceToHClip(float4 positionWS) {
    return mul(u_ViewProjection, positionWS);
}
