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

std::optional<Intersection> RayIntersectTriangle(const Ray& ray, const glm::vec3& pointA, const glm::vec3& pointB, const glm::vec3& pointC) {
    
    glm::vec3 edge1 = pointB - pointA;
    glm::vec3 edge2 = pointC - pointA;
    glm::vec3 h = glm::cross(ray.direction, edge2);
    float a = glm::dot(edge1, h);
    
    if (fabs(a) < 0.0000001f) return std::nullopt;
    float f = 1.0f / a;
    
    glm::vec3 s = ray.origin - pointA;
    float u = f * glm::dot(s, h);
    if (u < 0.0f || u > 1.0f) return std::nullopt;
    
    glm::vec3 q = glm::cross(s, edge1);
    float v = f * glm::dot(ray.direction, q);
    if (v < 0.0f || u + v > 1.0f) return std::nullopt;
    
    float t = f * glm::dot(edge2, q);
    if (t > 0.0000001f) {
        return Intersection{ray.origin + ray.direction * t, glm::vec3(0.0f), t};
    }
    
    return std::nullopt;
}

std::optional<Intersection> Raycast(const Ray& ray, const std::vector<float>& vertices, const std::vector<uint32_t>& indices) {
    
    std::optional<Intersection> closest;
    
    if (indices.size() != 0) {
        for (size_t i = 0; i < indices.size(); i += 3) {
            glm::vec3 pointA = glm::vec3(vertices[indices[i] * 3], vertices[indices[i] * 3 + 1], vertices[indices[i] * 3 + 2]);
            glm::vec3 pointB = glm::vec3(vertices[indices[i + 1] * 3], vertices[indices[i + 1] * 3 + 1], vertices[indices[i + 1] * 3 + 2]);
            glm::vec3 pointC = glm::vec3(vertices[indices[i + 2] * 3], vertices[indices[i + 2] * 3 + 1], vertices[indices[i + 2] * 3 + 2]);
            
            auto intersection = RayIntersectTriangle(ray, pointA, pointB, pointC);
            if (intersection) {
                if (!closest || intersection->distance < closest->distance) {
                    closest = intersection;
                    closest->normal = glm::normalize(glm::cross(pointB - pointA, pointC - pointA));
                }
            }
        }
    }
    else {
        for (int i = 0; i < vertices.size()/18; i++) {
            
            glm::vec3 pointA = glm::vec3(vertices[i * 18], vertices[i * 18 + 1], vertices[i * 18 + 2]);
            glm::vec3 pointB = glm::vec3(vertices[i * 18 + 3], vertices[i * 18 + 4], vertices[i * 18 + 5]);
            glm::vec3 pointC = glm::vec3(vertices[i * 18 + 6], vertices[i * 18 + 7], vertices[i * 18 + 8]);
            
            auto intersection = RayIntersectTriangle(ray, pointA, pointB, pointC);
            if (intersection) {
                if (!closest || intersection->distance < closest->distance) {
                    closest = intersection;
                    closest->normal = glm::normalize(glm::cross(pointB - pointA, pointC - pointA));
                }
            }
            
            glm::vec3 pointA2 = glm::vec3(vertices[i * 18 + 9], vertices[i * 18 + 10], vertices[i * 18 + 11]);
            glm::vec3 pointB2 = glm::vec3(vertices[i * 18 + 12], vertices[i * 18 + 13], vertices[i * 18 + 14]);
            glm::vec3 pointC2 = glm::vec3(vertices[i * 18 + 15], vertices[i * 18 + 16], vertices[i * 18 + 17]);
            
            intersection = RayIntersectTriangle(ray, pointA2, pointB2, pointC2);
            if (intersection) {
                if (!closest || intersection->distance < closest->distance) {
                    closest = intersection;
                    closest->normal = glm::normalize(glm::cross(pointB2 - pointA2, pointC2 - pointA2));
                }
            }
        }
    }
    return closest;
}

}

#endif /* raycast_h */
