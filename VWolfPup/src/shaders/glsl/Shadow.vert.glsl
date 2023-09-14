#version 400 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec3 a_Normal;
layout(location = 3) in vec3 a_Tangent;
layout(location = 4) in vec2 a_TexCoord;

layout(std140) uniform Camera
{
    mat4 u_ViewProjection;
};

layout(std140) uniform Object {
    mat4 u_Transform;
};

void main() {
    gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
}
