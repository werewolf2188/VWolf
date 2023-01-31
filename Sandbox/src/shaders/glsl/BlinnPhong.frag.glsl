#version 400 core
#define LIGHTS_MAX 8

layout(location = 0) out vec4 color;

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

uniform sampler2D u_texture;

in vec3 v_Position;
in vec3 v_Normal;
in vec2 v_TexCoord;

vec3 ComputeSpotBlinnPhongLightColor(vec4 v_LightPosition, vec4 v_LightDirection, int index, vec3 position, vec3 n) {
    // Ambient
    vec3 ambient = (u_ambientColor * light[index].u_color).xyz;
    
    vec3 s = normalize(v_LightPosition.xyz - position);
    float cosAng = dot(-s, normalize(v_LightDirection.xyz));
    float angle = acos( cosAng );
    float spotScale = 0.0;

    // Diffuse
    vec3 diffuse = vec3(0);
    
    // Specular
    vec3 spec = vec3(0);
    
    if (angle >= 0.0 && angle < light[index].u_cutOff) {
        spotScale = pow(cosAng, light[index].u_exponent);
        float sDotN = max(dot(s,n), 0.0);
        diffuse = (light[index].u_color * u_diffuseColor * sDotN).xyz;
        
        if (sDotN > 0.0) {
            vec3 v = normalize(-position.xyz);
            vec3 h = normalize(v + s);
            spec = u_specular * pow(max(dot(h,n), 0.0 ), u_shinines );
        }
    }
    
    return ambient + spotScale * light[index].u_strength.xyz * (diffuse + spec);
}

vec3 ComputeDirectionalBlinnPhonLightColor(vec4 v_LightPosition, vec4 v_LightDirection, int index, vec3 position, vec3 n) {
    // Ambient
    vec3 ambient = (u_ambientColor * light[index].u_color).xyz;

    // Diffuse
    vec3 s = normalize(light[index].u_direction.xyz);
    
    float sDotN = max(dot(s,n), 0.0);
    vec3 diffuse = (light[index].u_color * u_diffuseColor * sDotN).xyz;
    
    // Specular
    vec3 spec = vec3(0.0);
    if (sDotN > 0.0) {
        vec3 v = normalize(-position.xyz);
        vec3 h = normalize(v + s);
        spec = u_specular * pow(max(dot(h,n), 0.0 ), u_shinines );
    }

    return ambient + light[index].u_strength.xyz * (diffuse + spec);
}

vec3 ComputePoitBlinnPhongLightColor(vec4 v_LightPosition, vec4 v_LightDirection, int index, vec3 position, vec3 n) {
    // Ambient
    vec3 ambient = (u_ambientColor * light[index].u_color).xyz;

    // Diffuse
    vec3 s = normalize(v_LightPosition.xyz - position);
    
    float sDotN = max(dot(s,n), 0.0);
    vec3 diffuse = (light[index].u_color * u_diffuseColor * sDotN).xyz;
    
    // Specular
    vec3 spec = vec3(0.0);
    if (sDotN > 0.0) {
        vec3 v = normalize(-position.xyz);
        vec3 h = normalize(v + s);
        spec = u_specular * pow(max(dot(h,n), 0.0 ), u_shinines );
    }

    return ambient + light[index].u_strength.xyz * (diffuse + spec);
}

vec3 ComputeBlinnPhongLightColor(vec3 position, vec3 n) {
    
    vec3 color = vec3(0.0);
    
    for( int i = 0; i < LIGHTS_MAX; i++ ) {
        vec4 v_LightPosition = u_View * light[i].u_position;
        vec4 v_LightDirection = u_View * light[i].u_direction;
        if (light[i].u_type == 1) { // Directional
            color += ComputeDirectionalBlinnPhonLightColor(v_LightPosition, v_LightDirection, i, position, n);
        } else if (light[i].u_type == 2) { //Spotlight
            color += ComputeSpotBlinnPhongLightColor(v_LightPosition, v_LightDirection, i, position, n);
        } else if (light[i].u_type == 3) { //Point
            color += ComputePoitBlinnPhongLightColor(v_LightPosition, v_LightDirection, i, position, n);
        }
    }

    return color;
}

void main()
{
    vec4 sampleTexture = texture(u_texture, v_TexCoord);
    color = sampleTexture * vec4(ComputeBlinnPhongLightColor(v_Position, normalize(v_Normal)), 1.0);
}
