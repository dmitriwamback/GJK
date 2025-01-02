#version 410 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 projection;
uniform mat4 lookAt;
uniform mat4 model;

out prop {
    vec3 normal;
    vec3 fragp;
} vs_out;

void main() {
    vs_out.normal = transpose(inverse(mat3(model))) * normal;
    vs_out.fragp = vec3(model * vec4(position, 1.0));

    gl_Position = projection * lookAt * model * vec4(position, 1.0);
}