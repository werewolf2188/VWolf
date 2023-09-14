#version 400 core
#define LIGHTS_MAX 8

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec3 a_Normal;
layout(location = 3) in vec3 a_Tangent;
layout(location = 4) in vec2 a_TexCoord;

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

struct LightSpaceInfo {
    mat4 u_lightSpaceMatrix;
};

layout(std140) uniform LightSpace {
    LightSpaceInfo lightSpaces[LIGHTS_MAX];
};

out vec3 v_Position;
out vec3 v_Normal;
out vec2 v_TexCoord;
out vec4 v_FragPosLightSpace[LIGHTS_MAX];

void main()
{
    mat3 normalMatrix = mat3(u_View * u_Transform);
	v_Position = (u_View * u_Transform * vec4(a_Position, 1.0)).xyz;
    v_Normal = normalize(normalMatrix * a_Normal);
    v_TexCoord = a_TexCoord;
    vec4 fragPos = u_Transform * vec4(a_Position, 1.0);
    for( int i = 0; i < LIGHTS_MAX; i++ ) {
        v_FragPosLightSpace[i] = lightSpaces[i].u_lightSpaceMatrix * fragPos;
    }
	gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);	
}
