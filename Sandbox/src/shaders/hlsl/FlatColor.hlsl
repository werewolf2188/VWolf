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

cbuffer cbPerLight : register(b3) {
	//    uint u_type;
	float4 u_color;
	float3 u_direction;
	float3 u_position;
	float3 u_strength;
	float u_falloffStart;
	float u_falloffEnd;
	float u_spotPower;
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

/*
* OPENGL
vec4 ComputePhongLightColor() {
	mat3 normalMatrix = mat3(u_View * u_Transform);

	vec3 n = normalize(normalMatrix * a_Normal);
	vec4 camCoords = (u_View * u_Transform) * vec4(a_Position, 1.0);

	vec4 ambient = u_ambientColor * u_color;

	vec3 s = normalize(u_position - camCoords.xyz);
	float sDotN = max( dot(s,n), 0.0 );
	vec4 diffuse = u_color * u_diffuseColor * sDotN;

	vec3 spec = vec3(0.0);
	if( sDotN > 0.0 ) {
	  vec3 v = normalize(-camCoords.xyz);
	  vec3 r = reflect( -s, n );
	  spec = u_color.xyz * u_specular *
			  pow( max( dot(r,v), 0.0 ), u_shinines );
	}

	return ambient + diffuse + vec4(spec, 1.0);
}
*/

float4 ComputePhongLightColor(VertexIn vin) {
	float3x3 normalMatrix = (float3x3) (u_View * u_Transform);
	float3 n = normalize(mul(normalMatrix, vin.Normal));
	float4 camCoords = mul(mul(u_View, u_Transform), float4(vin.PosL, 1.0f));

	float4 ambient = u_ambientColor * u_color;

	float3 s = normalize(u_position - camCoords.xyz);
	float sDotN = max(dot(s, n), 0.0);
	float4 diffuse = u_color * u_diffuseColor * sDotN;

	float3 spec = float3(0.0, 0.0, 0.0);
	if (sDotN > 0.0) {
		float3 v = normalize(-camCoords.xyz);
		float3 r = reflect(-s, n);
		float maximum = max(dot(r, v), 0.0);
		float shiny = pow(maximum, u_shinines);
		spec = u_color.xyz * u_specular * shiny;
	}
	return ambient + diffuse + float4(spec, 1.0);
}

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
	vout.Color = ComputePhongLightColor(vin);//vin.Color;
	return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	return pin.Color;
}
