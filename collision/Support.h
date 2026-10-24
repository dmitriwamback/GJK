//
// Created by Dmitri on 2026-10-23.
//

#ifndef GJK_SUPPORT_H
#define GJK_SUPPORT_H
#include <glm/geometric.hpp>
#include <glm/vec3.hpp>

#include "../renderer/objects/Vertex.h"

inline glm::vec3 GetFurthestPoint(std::vector<Vertex> vertices, glm::vec3 direction) {

    glm::vec3 max = vertices[0].position;
    float distMax = glm::dot(direction, max);

    for (const Vertex& vertex : vertices) {
        glm::vec3 point = vertex.position;
        float dst = glm::dot(point, direction);

        if (dst > distMax) {
            distMax = dst;
            max = vertex.position;
        }
    }

    return max;
}

inline bool SameDirection(glm::vec3 direction, glm::vec3 AO) {
    return glm::dot(direction, AO) > 0;
}

inline glm::vec3 Support(const std::vector<Vertex>& vertices, glm::vec3 direction) {
    return GetFurthestPoint(vertices, direction);
}

#endif //GJK_SUPPORT_H
