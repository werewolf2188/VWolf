cbuffer cbPerCamera : register(b0)
{
	float4x4 u_ViewProjection;
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
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	// Transform to homogeneous clip space.
	vout.PosH = mul(u_Transform, float4(vin.PosL, 1.0f));
	vout.PosH = mul(u_ViewProjection, vout.PosH);
	vout.PosH.z = vout.PosH.z * 0.5 + 0.5;

	return vout;
}

void PS(VertexOut pin)
{
	float4 diffuseAlbedo = float4(0,0,0,0);
}
