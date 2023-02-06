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
	float3 PosL : a_Position;
	float3 Normal  : a_Normal;
};


float3 ComputeSpotBlinnPhongLightColor(float3 position, float3 n, LightInfo lightInfo) {
	
	// Light position
	float4 LightPosition = mul(u_View, lightInfo.u_position);

	// Light direction 
	float4 LightDirection = mul(u_View, lightInfo.u_direction);

	// Ambient
	float3 ambient = (u_ambientColor * lightInfo.u_color).xyz;

	
	float3 s = float3(0, 0, 0);
	s = normalize(LightPosition.xyz - position);
	float cosAng = dot(-s, normalize(LightDirection.xyz));
	float angle = acos(cosAng);
	float spotScale = 0.0;

	// Diffuse
	float3 diffuse = float3(0.0, 0.0, 0.0);

	// Spec
	float3 spec = float3(0.0, 0.0, 0.0);

	if (angle >= 0.0 && angle < lightInfo.u_cutOff) {
		spotScale = pow(cosAng, lightInfo.u_exponent);
		float sDotN = max(dot(s, n), 0.0);
		diffuse = (lightInfo.u_color * u_diffuseColor * sDotN).xyz;

		// Specular
		if (sDotN > 0) {
			float3 v = normalize(-position.xyz);
			float3 h = normalize(v + s);
			spec = u_specular * pow(max(dot(h, n), 0.0), u_shinines);
		}
	}

	return ambient + spotScale * lightInfo.u_strength.xyz * (diffuse + spec);
}


float3 ComputePointBlinnPhongLightColor(float3 position, float3 n, LightInfo lightInfo) {
	// Light position
	float4 LightPosition = mul(u_View, lightInfo.u_position);

	// Ambient
	float3 ambient = (u_ambientColor * lightInfo.u_color).xyz;

	// Diffuse
	float3 s = float3(0, 0, 0);
	s = normalize(LightPosition.xyz - position);
	float sDotN = max(dot(s, n), 0.0);
	float3 diffuse = (lightInfo.u_color * u_diffuseColor * sDotN).xyz;

	// Specular
	float3 spec = float3(0.0, 0.0, 0.0);
	if (sDotN > 0) {
		float3 v = normalize(-position.xyz);
		float3 h = normalize(v + s);
		spec = u_specular * pow(max(dot(h, n), 0.0), u_shinines);
	}

	return ambient + lightInfo.u_strength.xyz * (diffuse + spec);
}

float3 ComputeDirectionalBlinnPhongLightColor(float3 position, float3 n, LightInfo lightInfo) {

	// Ambient
	float3 ambient = (u_ambientColor * lightInfo.u_color).xyz;

	// Diffuse
	float3 s = float3(0, 0, 0);
	s = normalize(lightInfo.u_direction.xyz);
	float sDotN = max(dot(s, n), 0.0);
	float3 diffuse = (lightInfo.u_color * u_diffuseColor * sDotN).xyz;

	// Specular
	float3 spec = float3(0.0, 0.0, 0.0);
	if (sDotN > 0) {
		float3 v = normalize(-position.xyz);
		float3 h = normalize(v + s);
		spec = u_specular * pow(max(dot(h, n), 0.0), u_shinines);
	}

	return ambient + lightInfo.u_strength.xyz * (diffuse + spec);
}

float3 ComputeBlinnPhongLightColor(float3 position, float3 n) {
	float3 color = float3(0.0, 0.0, 0.0);

	for (int i = 0; i < LIGHTS_MAX; i++) {
		if (light[i].u_type == 1) { // Directional
			color += ComputeDirectionalBlinnPhongLightColor(position, n, light[i]);
		}
		else if (light[i].u_type == 2) { // Spotlight
			color += ComputeSpotBlinnPhongLightColor(position, n, light[i]);
		}
		else if (light[i].u_type == 3) { // Point
			color += ComputePointBlinnPhongLightColor(position, n, light[i]);
		}
	}
	return color;
}



VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	// Transform to homogeneous clip space.
	vout.PosH = mul(u_Transform, float4(vin.PosL, 1.0f));
	vout.PosH = mul(u_ViewProjection, vout.PosH);

	// Just pass vertex color into the pixel shader.	
	float3x3 normalMatrix = (float3x3)mul(u_View, u_Transform);
	vout.PosL = mul(u_Transform, float4(vin.PosL, 1.0f)).xyz;
	vout.PosL = mul(u_View, float4(vout.PosL, 1.0f)).xyz;
	vout.Normal = normalize(mul(normalMatrix, vin.Normal));
	
	return vout;
}

float4 PS(VertexOut pin) : SV_Target
{

	return float4(ComputeBlinnPhongLightColor(pin.PosL, normalize(pin.Normal)), 1.0);
}
