#version 400 core

layout(location = 0) out vec4 color;

in vec3 v_Position;
in vec4 v_Color;

void main()
{
//	color = vec4(1.0, 0.0, 0.0, 1.0);
	color = v_Color;
}
