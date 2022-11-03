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

layout(std140) uniform Light {
    vec4 u_color;
    vec3 u_position;
    vec3 u_direction;
    vec3 u_strength;
    float u_falloffStart;
    float u_falloffEnd;
    float u_spotPower;
    uint u_type;
};

out vec3 v_Position;
out vec4 v_Color;

void main()
{
	v_Position = a_Position;
	v_Color = u_color;
	gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);	
}
