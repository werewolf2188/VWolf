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
    vec4 u_position;
    vec4 u_direction;
    vec4 u_strength;
    float u_cutOff;
    float u_exponent;
    uint u_type;
};

in vec3 v_Position;
in vec3 v_Normal;

in vec4 v_LightPosition;
in vec4 v_LightDirection;

vec3 ComputeSpotBlinnPhongLightColor(vec3 position, vec3 n) {
    // Ambient
    vec3 ambient = (u_ambientColor * u_color).xyz;
    
    vec3 s = normalize(v_LightPosition.xyz - position);
    float cosAng = dot(-s, normalize(v_LightDirection.xyz));
    float angle = acos( cosAng );
    float spotScale = 0.0;

    // Diffuse
    vec3 diffuse = vec3(0);
    
    // Specular
    vec3 spec = vec3(0);
    
    if (angle >= 0.0 && angle < u_cutOff) {
        spotScale = pow(cosAng, u_exponent);
        float sDotN = max(dot(s,n), 0.0);
        diffuse = (u_color * u_diffuseColor * sDotN).xyz;
        
        if (sDotN > 0.0) {
            vec3 v = normalize(-position.xyz);
            vec3 h = normalize(v + s);
            spec = u_specular * pow(max(dot(h,n), 0.0 ), u_shinines );
        }
    }
    
    return ambient + spotScale * u_strength.xyz * (diffuse + spec);
}

vec3 ComputeDirectionalBlinnPhonLightColor(vec3 position, vec3 n) {
    // Ambient
    vec3 ambient = (u_ambientColor * u_color).xyz;

    // Diffuse
    vec3 s = normalize(u_direction.xyz);
    
    float sDotN = max(dot(s,n), 0.0);
    vec3 diffuse = (u_color * u_diffuseColor * sDotN).xyz;
    
    // Specular
    vec3 spec = vec3(0.0);
    if (sDotN > 0.0) {
        vec3 v = normalize(-position.xyz);
        vec3 h = normalize(v + s);
        spec = u_specular * pow(max(dot(h,n), 0.0 ), u_shinines );
    }

    return ambient + u_strength.xyz * (diffuse + spec);
}

vec3 ComputePoitBlinnPhongLightColor(vec3 position, vec3 n) {
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

    return ambient + u_strength.xyz * (diffuse + spec);
}

vec3 ComputeBlinnPhongLightColor(vec3 position, vec3 n) {
    
    if (u_type == 1) { // Directional
        return ComputeDirectionalBlinnPhonLightColor(position, n);
    } else if (u_type == 2) { //Spotlight
        return ComputeSpotBlinnPhongLightColor(position, n);
    } else if (u_type == 3) { //Point
        return ComputePoitBlinnPhongLightColor(position, n);
    }
    return ComputePoitBlinnPhongLightColor(position, n);
}

void main()
{
	color = vec4(ComputeBlinnPhongLightColor(v_Position, normalize(v_Normal)), 1.0);
}
