#version 450 core
layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

in vec3 vsWorldPos[];
in float vsSize[];
in float vsLife[];

out vec2 gUV;
out float gLife;

uniform mat4 view;
uniform mat4 proj;
uniform vec3 camRight; // camera right in world space
uniform vec3 camUp;    // camera up in world space

void main() {
    vec3 worldPos = vsWorldPos[0];
    float size = vsSize[0];
    gLife = vsLife[0];

    vec3 r = normalize(camRight) * size;
    vec3 u = normalize(camUp) * size;

    vec3 p0 = worldPos - r - u;
    vec3 p1 = worldPos + r - u;
    vec3 p2 = worldPos - r + u;
    vec3 p3 = worldPos + r + u;

    gUV = vec2(0.0, 0.0);
    gl_Position = proj * view * vec4(p0, 1.0);
    EmitVertex();

    gUV = vec2(1.0, 0.0);
    gl_Position = proj * view * vec4(p1, 1.0);
    EmitVertex();

    gUV = vec2(0.0, 1.0);
    gl_Position = proj * view * vec4(p2, 1.0);
    EmitVertex();

    gUV = vec2(1.0, 1.0);
    gl_Position = proj * view * vec4(p3, 1.0);
    EmitVertex();

    EndPrimitive();
}
