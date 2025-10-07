#version 450 core
in vec2 gUV;
in float gLife;

out vec4 FragColor;

uniform sampler2D smokeTex;
uniform vec3 tint; // small color tint

void main() {
    vec4 t = texture(smokeTex, gUV);
    // gLife: 0 = new, 1 = old -> fade out as life increases
    float lifeFade = 1.0 - gLife;
    float alpha = t.a * lifeFade;
    if (alpha < 0.01) discard;
    vec3 color = mix(vec3(1.0), tint, 0.2) * t.rgb;
    FragColor = vec4(color, alpha);
}
