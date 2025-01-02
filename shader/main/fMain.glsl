#version 410 core

out vec4 fragc;

uniform vec3 color;

in prop {
    vec3 normal;
    vec3 fragp;
} fs_in;

void main() {
    fragc = vec4(color, 1.0);
}