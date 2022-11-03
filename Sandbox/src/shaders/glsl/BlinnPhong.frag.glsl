#version 400 core

layout(location = 0) out vec4 color;

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

in vec3 v_Position;
in vec3 v_Normal;

in vec4 v_LightPosition;

vec3 ComputeBlinnPhongLightColor(vec3 position, vec3 n) {
    
    // Ambient
    vec3 ambient = (u_ambientColor * u_color).xyz;

    // Diffuse
    vec3 s = normalize(v_LightPosition.xyz - position);
    float sDotN = max(dot(s,n), 0.0);
    vec3 diffuse = (u_color * u_diffuseColor * sDotN).xyz;
    
    // Specular
    vec3 spec = vec3(0.0);
    if (sDotN > 0.0) {
        vec3 v = normalize(-position.xyz);
        vec3 h = normalize(v + s);
        spec = u_specular * pow(max(dot(h,n), 0.0 ), u_shinines );
    }

    return ambient + diffuse + spec;
}

void main()
{
	color = vec4(ComputeBlinnPhongLightColor(v_Position, normalize(v_Normal)), 1.0);
}
