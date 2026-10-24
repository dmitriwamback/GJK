//
// Created by Dmitri on 2026-10-23.
//

#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

#include "../../util/MathUtil.h"

Camera::Camera() {
    position = glm::vec3(0.0f, 6.0f, 0.0f);
    lookDirection = glm::vec3(0.0f, 0.0f, 1.0f);

    projection = glm::perspective(3.14159265358f/2.0f, 3.0f/2.0f, 0.1f, 1000.0f);
    upDirection = glm::vec3(0.0f, 1.0f, 0.0f);

    vertices = {
        Vertex({-0.5f,  0.5f,  0.5f}, glm::vec3(0.0f), glm::vec2(0.0f)),
        Vertex({ 0.5f,  0.5f,  0.5f}, glm::vec3(0.0f), glm::vec2(0.0f)),
        Vertex({ 0.5f, -0.5f,  0.5f}, glm::vec3(0.0f), glm::vec2(0.0f)),
        Vertex({-0.5f, -0.5f,  0.5f}, glm::vec3(0.0f), glm::vec2(0.0f)),
        Vertex({-0.5f,  0.5f, -0.5f}, glm::vec3(0.0f), glm::vec2(0.0f)),
        Vertex({ 0.5f,  0.5f, -0.5f}, glm::vec3(0.0f), glm::vec2(0.0f)),
        Vertex({ 0.5f, -0.5f, -0.5f}, glm::vec3(0.0f), glm::vec2(0.0f)),
        Vertex({-0.5f, -0.5f, -0.5f}, glm::vec3(0.0f), glm::vec2(0.0f)),
    };
}

glm::vec3 Camera::CalculateVelocity(glm::vec4 movement, float up, float down) const {

    float forward   = movement.x,
          backward  = movement.y,
          left      = movement.z,
          right     = movement.w;

    glm::vec3 motion = lookDirection;
    glm::vec3 rightMotion = glm::normalize(glm::cross(motion, upDirection));

    float forwardMotion  = forward + backward,
          sidewaysMotion = left + right,
          verticalMotion = up + down;

    glm::vec3 vStraight = motion * forwardMotion * speed;
    glm::vec3 vRight    = rightMotion * sidewaysMotion * speed;
    glm::vec3 vUp       = upDirection * verticalMotion * speed;

    return vStraight - vRight + vUp;
}

void Camera::Update(glm::vec4 movement, float up, float down, GLFWwindow* window) {
    glm::vec3 velocity = CalculateVelocity(movement, up, down);
    position += velocity;

    lookDirection = glm::normalize(glm::vec3(cos(yaw) * cos(pitch), sin(pitch), sin(yaw) * cos(pitch)));
    lookAt = glm::lookAt(position, position + lookDirection, upDirection);

    int width, height;
    glfwGetWindowSize(window, &width, &height);
    float aspect = (float)width / (float)height;

    projection = glm::perspective(3.14159265358f / 2.0f, aspect, 0.1f, 1000.0f);
}

void Camera::UpdateLookAtMatrix() {
    lookAt = glm::lookAt(position, position + lookDirection, upDirection);
}

std::vector<Vertex> Camera::GetTransformedVertices() {
    glm::mat4 model = MathUtil::ComputeModelMatrix(position, glm::vec3(0.0f), glm::vec3(0.5f));

    std::vector<Vertex> transformedVertices;
    for (Vertex vertex : vertices) {
        Vertex v{};

        glm::vec4 transformedPosition = model * glm::vec4(vertex.position, 1.0f);

        v.position = glm::vec3(transformedPosition.x, transformedPosition.y, transformedPosition.z);
        transformedVertices.push_back(v);
    }

    return transformedVertices;
}

Ray Camera::GetMouseRay(int windowWidth, int windowHeight) {
    float x = 2.0f * lastMouseX / (float)windowWidth - 1.0f;
    float y = 1.0f - (2.0f * lastMouseY) / (float)windowHeight;

    glm::vec3 nearPlane = glm::vec3(x, y, 0.0f);
    glm::vec3 farPlane = glm::vec3(x, y, 1.0f);

    glm::mat4 inverseProjection = glm::inverse(projection);

    glm::vec4 nearWorld = inverseProjection * glm::vec4(nearPlane, 1.0f);
    glm::vec4 farWorld = inverseProjection * glm::vec4(farPlane, 1.0f);
    nearWorld /= nearWorld.w;
    farWorld /= farWorld.w;

    glm::vec3 rayDirection = glm::normalize(glm::vec3(farWorld) - glm::vec3(nearWorld));
    glm::vec3 mouseRayDirection = glm::normalize(glm::vec3(glm::inverse(lookAt) * glm::vec4(rayDirection, 0.0f)));

    Ray ray{};
    ray.origin = position;
    ray.direction = mouseRayDirection;

    return ray;
}
