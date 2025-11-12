#version 330 core

out vec4 FragColor;

in vec3 fragPos;  // position of the fragment
in vec3 normal;   // normal vector at the fragment

uniform vec3 lightPos;    // position of the light
uniform vec3 viewPos;     // camera position
uniform vec3 ambientColor; // ambient light color

void main()
{
    // Ambient lighting calculation
    vec3 ambient = ambientColor;  // scale ambient color by light intensity

    // Combine ambient light with other lighting (like diffuse or specular)
    FragColor = vec4(ambient, 1.0f);
}
