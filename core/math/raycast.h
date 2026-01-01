//
//  raycast.h
//  GJK
//
//  Created by Dmitri Wamback on 2025-01-03.
//

#ifndef raycast_h
#define raycast_h

#include <optional>

namespace core {

struct Ray {
    glm::vec3 origin;
    glm::vec3 direction;
};

struct Intersection {
    glm::vec3 intersectionPoint;
    glm::vec3 normal;
    float distance;
};

struct Triangle {
    
    glm::vec3 a, b, c;
    glm::vec3 normal;
    glm::vec3 minBound;
    glm::vec3 maxBound;

    Triangle(const glm::vec3& pa, const glm::vec3& pb, const glm::vec3& pc) : a(pa), b(pb), c(pc) {
        normal = glm::normalize(glm::cross(b - a, c - a));
        minBound = glm::min(glm::min(a, b), c);
        maxBound = glm::max(glm::max(a, b), c);
    }
};

inline bool RayAABBIntersect(const Ray& ray, const glm::vec3& min, const glm::vec3& max) {
    
    glm::vec3 invDir = 1.0f / ray.direction;
    glm::vec3 t0s = (min - ray.origin) * invDir;
    glm::vec3 t1s = (max - ray.origin) * invDir;
    glm::vec3 tsmaller = glm::min(t0s, t1s);
    glm::vec3 tbigger  = glm::max(t0s, t1s);
    
    float tmin = std::max({tsmaller.x, tsmaller.y, tsmaller.z});
    float tmax = std::min({tbigger.x, tbigger.y, tbigger.z});
    return tmax >= std::max(tmin, 0.0f);
}

inline std::optional<float> RayIntersectTriangle(const Ray& ray, const Triangle& tri) {
    
    glm::vec3 edge1 = tri.b - tri.a;
    glm::vec3 edge2 = tri.c - tri.a;
    glm::vec3 h = glm::cross(ray.direction, edge2);
    
    float a = glm::dot(edge1, h);
    if (glm::abs(a) < 1e-7f) return std::nullopt;
    float f = 1.0f / a;
    
    glm::vec3 s = ray.origin - tri.a;
    float u = f * glm::dot(s, h);
    if (u < 0.0f || u > 1.0f) return std::nullopt;
    
    glm::vec3 q = glm::cross(s, edge1);
    float v = f * glm::dot(ray.direction, q);
    if (v < 0.0f || u + v > 1.0f) return std::nullopt;
    float t = f * glm::dot(edge2, q);
    
    return t > 1e-7f ? std::optional<float>(t) : std::nullopt;
}

inline std::optional<Intersection> Raycast(const Ray& ray, const std::vector<Triangle>& triangles) {
    std::optional<Intersection> closest;
    float closestDist = std::numeric_limits<float>::max();

    for (const auto& tri : triangles) {
        if (!RayAABBIntersect(ray, tri.minBound, tri.maxBound)) continue;
        auto t = RayIntersectTriangle(ray, tri);
        if (t && *t < closestDist) {
            closestDist = *t;
            closest = Intersection{ray.origin + ray.direction * *t, tri.normal, *t};
        }
    }

    return closest;
}

} // namespace core

#endif /* raycast_h */
