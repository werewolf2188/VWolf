#version 400 core

layout(location = 0) out vec4 outColor;

in vec3 texCoords;

uniform samplerCube skybox;

void main() {    
    outColor = texture(skybox, texCoords);;
}
