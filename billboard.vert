#version 450 core
layout (location = 0) in vec3 inPos;
layout (location = 1) in float inSize;
layout (location = 2) in float inLife; // 0..1 life ratio

out vec3 vsWorldPos;
out float vsSize;
out float vsLife;

uniform mat4 model;

void main() {
    vec4 worldPos = model * vec4(inPos, 1.0);
    vsWorldPos = worldPos.xyz;
    vsSize = inSize;
    vsLife = inLife;
    // we don't set final clip pos here — geometry shader will project from world-space
    gl_Position = worldPos;
}
