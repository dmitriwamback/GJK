//
// Created by Dmitri on 2026-10-23.
//

#include "Raycast.h"

#include <__float_float.h>

bool Raycast::RayAABBIntersect(const Ray &ray, const glm::vec3 &min, const glm::vec3 &max) {

    glm::vec3 inverseDirection = 1.0f / ray.direction;

    glm::vec3 t0s = (min - ray.origin) * inverseDirection;
    glm::vec3 t1s = (max - ray.origin) * inverseDirection;
    glm::vec3 tsmaller = glm::min(t0s, t1s);
    glm::vec3 tbigger = glm::max(t0s, t1s);

    float tmin = std::max({tsmaller.x, tsmaller.y, tsmaller.z});
    float tmax = std::min({tbigger.x, tbigger.y, tbigger.z});

    return tmax >= std::max(tmin, 0.0f);
}

std::optional<float> Raycast::RayIntersectTriangle(const Ray &ray, const Triangle &triangle) {

    glm::vec3 edge1 = triangle.v1 - triangle.v0;
    glm::vec3 edge2 = triangle.v2 - triangle.v0;

    glm::vec3 h = glm::cross(ray.direction, edge2);

    float a = glm::dot(edge1, h);
    if (glm::abs(a) < 0.00000001f) return std::nullopt;

    float f = 1.0f / a;
    glm::vec3 s = ray.origin - triangle.v0;

    float u = f * glm::dot(s, h);
    if (u < 0.0f || u > 1.0f) return std::nullopt;

    glm::vec3 q = glm::cross(s, edge1);
    float v = f * glm::dot(ray.direction, q);
    if (v < 0.0f || u + v > 1.0f) return std::nullopt;

    float t = f * glm::dot(edge2, q);

    return t > 0.00000001f ? std::optional<float>(t) : std::nullopt;
}

std::optional<Intersection> Raycast::PerformRaycast(const Ray &ray, const std::vector<Triangle> &triangles) {
    std::optional<Intersection> closest;

    float closestDst = FLT_MAX;

    for (const Triangle &triangle : triangles) {
        if (!RayAABBIntersect(ray, triangle.minBound, triangle.maxBound)) continue;

        auto t = RayIntersectTriangle(ray, triangle);
        if (t && *t < closestDst) {
            closestDst = *t;
            closest = Intersection(ray.origin + ray.direction * *t, triangle.normal, *t);
        }
    }

    return closest;
}
