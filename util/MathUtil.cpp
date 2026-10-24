//
// Created by Dmitri on 2026-10-23.
//

#include "MathUtil.h"

glm::mat4 MathUtil::ComputeModelMatrix(const glm::vec3 &position, const glm::vec3 &rotation, const glm::vec3 &size) {

    glm::mat4 translation = glm::translate(glm::mat4(1.0f), position);

    glm::mat4 xRotation = glm::rotate(glm::mat4(1.0f), rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 yRotation = glm::rotate(glm::mat4(1.0f), rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 zRotation = glm::rotate(glm::mat4(1.0f), rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

    glm::mat4 rot = xRotation * yRotation * zRotation;
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), size);

    return translation * rot * scale;
}

bool MathUtil::isDegenerate(Triangle triangle) {
    glm::vec3 b1 = triangle.v0 - triangle.v1;
    glm::vec3 b2 = triangle.v0 - triangle.v2;

    return glm::length(glm::cross(b1, b2)) <= 0.0f;
}
