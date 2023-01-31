#version 400 core
// Shared set between most vertex shaders
layout(std140) uniform ViewUniforms {
    mat4 view;
    mat4 proj;
    vec3 pos;
} view;

layout (std140) uniform NearFarPoint {
    float near; //0.01
    float far; //100
} nfPoint;

out vec3 nearPoint;
out vec3 farPoint;
out mat4 fragView;
out mat4 fragProj;
out float near; //0.01
out float far; //100

vec3 UnprojectPoint(float x, float y, float z, mat4 view, mat4 projection) {
    mat4 viewInv = inverse(view);
    mat4 projInv = inverse(projection);
    vec4 unprojectedPoint =  viewInv * projInv * vec4(x, y, z, 1.0);
    return unprojectedPoint.xyz / unprojectedPoint.w;
}

// Grid position are in xy clipped space
vec3 gridPlane[6] = vec3[](
    vec3(1, 1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
    vec3(-1, -1, 0), vec3(1, 1, 0), vec3(1, -1, 0)
);
// normal vertice projection
void main() {
    vec3 p = gridPlane[gl_VertexID].xyz;
    nearPoint = UnprojectPoint(p.x, p.y, 0.0, view.view, view.proj).xyz; // unprojecting on the near plane
    farPoint = UnprojectPoint(p.x, p.y, 1.0, view.view, view.proj).xyz; // unprojecting on the far plane
    fragView = view.view;
    fragProj = view.proj;
    near = nfPoint.near;
    far = nfPoint.far;
    gl_Position = vec4(p, 1.0); // using directly the clipped coordinates
}
