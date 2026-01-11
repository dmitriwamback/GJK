#version 410 core

out vec4 fragc;

uniform vec3 color;
vec3 lightPosition = vec3(10000.0);

in prop {
    vec3 normal;
    vec3 fragp;
} fs_in;

void main() {

    vec3 ambient = color * 0.4;

    vec3 lightDirection = normalize(lightPosition - fs_in.fragp);
    float diff = max(dot(fs_in.normal, lightDirection), 0.0);
    vec3 diffuse = diff * vec3(1.0);

    fragc = vec4(color * (ambient + diffuse), 1.0);
}