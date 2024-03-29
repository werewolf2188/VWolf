#version 400 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec3 a_Normal;
layout(location = 3) in vec3 a_Tangent;
layout(location = 2) in vec2 a_TexCoord;

layout(std140) uniform Object {
    mat4 u_Transform;
};

layout(std140) uniform Camera
{
    /*
        TODO: I don't know what this value does
         float cbPerObjectPad1;
     */
    mat4 u_View;
    mat4 u_InvView;
    mat4 u_Proj;
    mat4 u_InvProj;
    mat4 u_ViewProjection;
    mat4 u_InvViewProjection;
    vec3 u_EyePosition;
    vec2 u_RenderTargetSize;
    vec2 u_InvRenderTargetSize;
    float u_NearZ;
    float u_FarZ;
    float u_TotalTime;
    float u_DeltaTime;
};

layout(std140) uniform Material {
    vec4 u_ambientColor;
    vec4 u_diffuseColor;
    vec3 u_specular;
    float u_shinines;
};

struct LightInfo {
    vec4 u_color;
    vec4 u_position;
    vec4 u_direction;
    vec4 u_strength;
    float u_cutOff;
    float u_exponent;
    uint u_type;
};

layout(std140) uniform Light {
    LightInfo light[LIGHTS_MAX];
};

out vec3 v_Position;
out vec4 v_Color;

vec4 ComputePhongLightColor() {
    mat3 normalMatrix = mat3(u_View * u_Transform);
    
    vec3 n = normalize(normalMatrix * a_Normal);
    vec4 camCoords = (u_View * u_Transform) * vec4(a_Position, 1.0);
    
    vec4 ambient = u_ambientColor * light[0].u_color;
    
    vec4 lightPosition = u_View * light[0].u_position;
    
    vec3 s = normalize(vec3(lightPosition - camCoords));
    float sDotN = max( dot(s,n), 0.0 );
    vec4 diffuse = light[0].u_color * u_diffuseColor * sDotN;
    
    vec3 spec = vec3(0.0);
    if( sDotN > 0.0 ) {
      vec3 v = normalize(-camCoords.xyz);
      vec3 r = reflect( -s, n );
      spec = u_specular *
              pow( max( dot(r,v), 0.0 ), u_shinines );
    }

    return ambient + diffuse + vec4(spec, 1.0);
}

void main()
{
	v_Position = a_Position;
	v_Color = ComputePhongLightColor();
	gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);	
}
