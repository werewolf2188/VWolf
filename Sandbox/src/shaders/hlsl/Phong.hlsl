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

cbuffer cbPerMaterial : register(b3) {
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

cbuffer cbPerLight : register(b2) {
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

float4 ComputePhongLightColor(VertexIn vin) {
	float3x3 normalMatrix = (float3x3)mul(u_View, u_Transform);

	float3 n = normalize(mul(normalMatrix, vin.Normal));

	float4 camCoords = mul(mul(u_View, u_Transform), float4(vin.PosL, 1.0f));

	float4 ambient = u_ambientColor * light[0].u_color;
	float4 lightPosition = mul(u_View, light[0].u_position);

	float3 s = normalize((lightPosition - camCoords).xyz);
	float sDotN = max(dot(s, n), 0.0);
	float4 diffuse = light[0].u_color * u_diffuseColor * sDotN;

	float3 spec = float3(0.0, 0.0, 0.0);
	if (sDotN > 0.0) {
		float3 v = normalize(-camCoords.xyz);
		float3 r = reflect(-s, n);
		float maximum = max(dot(r, v), 0.0);
		float shiny = pow(maximum, u_shinines);
		spec = light[0].u_color.xyz * u_specular * shiny;
	}
	return ambient + diffuse + float4(spec, 1.0);
}

VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	// Transform to homogeneous clip space.
	vout.PosH = mul(u_Transform, float4(vin.PosL, 1.0f));
	vout.PosH = mul(u_ViewProjection, vout.PosH);

	// Just pass vertex color into the pixel shader.	
	vout.Color = ComputePhongLightColor(vin);//vin.Color;
	return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	return pin.Color;
}
