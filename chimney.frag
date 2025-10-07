#version 450 core
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D tex;
uniform vec3 lightDir;   // should be normalized
uniform vec3 lightColor;
uniform vec3 ambient;

void main() {
    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, -lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 baseColor = texture(tex, TexCoord).rgb;
    vec3 result = (ambient + diffuse) * baseColor;

    FragColor = vec4(result, 1.0);
}
