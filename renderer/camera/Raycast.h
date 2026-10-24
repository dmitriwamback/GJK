//
// Created by Dmitri on 2026-10-23.
//

#ifndef GJK_RAYCAST_H
#define GJK_RAYCAST_H
#include <glm/vec3.hpp>

#include "../../util/MathUtil.h"

struct Ray {
    glm::vec3 origin;
    glm::vec3 direction;
};

struct Intersection {
    glm::vec3 intersectionPoint;
    glm::vec3 normal;
    float distance;
};

class Raycast {
public:
    static bool RayAABBIntersect(const Ray& ray, const glm::vec3& min, const glm::vec3& max);
    static std::optional<float> RayIntersectTriangle(const Ray& ray, const Triangle& triangle);
    static std::optional<Intersection> PerformRaycast(const Ray& ray, const std::vector<Triangle>& triangles);
};


#endif //GJK_RAYCAST_H
