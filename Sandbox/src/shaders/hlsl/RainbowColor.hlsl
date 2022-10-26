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

	// GLM
	// Working
	/*float4x4 vp = {
		41.0274, 0, 0, 0,
		0, 72.9467, 0, 0,
		0, 0, -1.0002, -1,
		0, 0, 49.81, 50
	};*/

	// Tests to make sure the constant buffer is passing or not
	//vout.PosH = mul(float4(vin.PosL, 1.0f), vp);
	//vout.PosH = float4(vin.PosL, 1.0f);
	//float4 co = { u_ViewProjection._11 / 0xff, u_ViewProjection._22 / 0xff, u_ViewProjection._43 / 0xff, u_ViewProjection._44 / 0xff };
	//vout.Color = co;

	// Transform to homogeneous clip space.
	vout.PosH = mul(u_Transform, float4(vin.PosL, 1.0f));
	vout.PosH = mul(u_ViewProjection, vout.PosH);

	// Just pass vertex color into the pixel shader.	
	vout.Color = vin.Color;
	return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	return pin.Color;
}
