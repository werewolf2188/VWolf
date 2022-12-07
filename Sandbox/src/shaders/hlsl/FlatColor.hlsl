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

cbuffer cbPerObject : register(b1)
{
	float4x4 u_Transform;
};

cbuffer cbPerMaterial : register(b2) {
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

cbuffer cbPerLight : register(b3) {
	LightInfo light[LIGHTS_MAX];
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
	float4 Color : a_Color;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	// Transform to homogeneous clip space.
	vout.PosH = mul(u_Transform, float4(vin.PosL, 1.0f));
	vout.PosH = mul(u_ViewProjection, vout.PosH);

	// Just pass vertex color into the pixel shader.	
	vout.Color = light[0].u_color;
	return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	return pin.Color;
}
