//
// Created by Dmitri on 2026-10-23.
//

#ifndef GJK_MATHUTIL_H
#define GJK_MATHUTIL_H

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Triangle {
    glm::vec3 v0, v1, v2;
    glm::vec3 normal;
    glm::vec3 minBound, maxBound;

    Triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c) {
        v0 = a;
        v1 = b;
        v2 = c;

        normal = glm::normalize(glm::cross(b - a, c - a));
        minBound = glm::min(glm::min(a, b), c);
        maxBound = glm::max(glm::max(a, b), c);
    }
};

class MathUtil {
public:
    static glm::mat4 ComputeModelMatrix(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& size);
    static bool isDegenerate(Triangle triangle);
};


#endif //GJK_MATHUTIL_H
