cbuffer cbPerCamera : register(b0)
{
	float4x4 u_ViewProjection;
};

cbuffer cbPerObject : register(b1)
{
	float4x4 u_Transform;
};

struct VertexIn
{
	float3 PosL  : a_Position;
	float4 Color : a_Color;
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
