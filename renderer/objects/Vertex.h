//
// Created by Dmitri on 2026-10-23.
//

#ifndef GJK_VERTEX_H
#define GJK_VERTEX_H

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
};

#endif //GJK_VERTEX_H
