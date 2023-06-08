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

struct VertexIn
{
    float3 PosL  : a_Position;
    float4 Color : a_Color;
    float3 Normal  : a_Normal;
    float3 Tangent  : a_Tangent;
    float2 TexCoord  : a_TexCoord;
};

struct VertexOut
{
    float4 PosH  : SV_POSITION;
    float3 TexC    : TEXCOORD;
};

TextureCube  skybox : register(t1);

SamplerState gsamPointWrap : register(s0);

VertexOut VS(VertexIn vin)
{
    VertexOut vOut;
	float4 pos = mul(u_View, float4(vin.PosL, 1.0f));
	pos = mul(u_Proj, pos);

	// Having z equal w will always result in a depth of 1.0f
	vOut.PosH = float4(pos.x, pos.y, pos.w, pos.w);
	// We want to flip the z axis due to the different coordinate systems (left hand vs right hand)
	vOut.TexC = float3(vin.PosL.x, vin.PosL.y, -vin.PosL.z);
	
	return vOut;
}

float4 PS(VertexOut pin): SV_Target{
   
	float4 sampleTexture = skybox.Sample(gsamPointWrap, pin.TexC);
	return sampleTexture;
}