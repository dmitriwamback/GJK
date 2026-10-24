//
// Created by Dmitri on 2026-10-23.
//

#ifndef GJK_SHADERSOURCES_H
#define GJK_SHADERSOURCES_H


const char* DEFAULT_VERTEX_SHADER_SOURCE = R"(
#version 410 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

uniform mat4 projection;
uniform mat4 lookAt;
uniform mat4 model;

out prop {
    vec3 normal;
    vec3 fragp;
} vs_out;

void main() {
    vs_out.normal = normalize(transpose(inverse(mat3(model))) * normal);
    vs_out.fragp = vec3(model * vec4(position, 1.0));

    gl_Position = projection * lookAt * model * vec4(position, 1.0);
}
)";


const char* DEFAULT_FRAGMENT_SHADER_SOURCE = R"(
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
)";

#endif //GJK_SHADERSOURCES_H
