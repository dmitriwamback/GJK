//
// Created by Dmitri on 2026-10-23.
//

#ifndef GJK_CAMERA_H
#define GJK_CAMERA_H

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <GLFW/glfw3.h>

#include "Raycast.h"
#include "../objects/Vertex.h"

class Camera {
public:
    glm::vec3 position;
    glm::vec3 lookDirection;
    glm::vec3 upDirection;
    std::vector<Vertex> vertices;

    glm::mat4 projection, lookAt;

    float yaw = 0, pitch = 0;
    float speed = 40.0f;

    float lastMouseX, lastMouseY;
    int mouseButton = GLFW_MOUSE_BUTTON_RIGHT;

    Camera();

    void Update(glm::vec4 movement, float up, float down, GLFWwindow* window, float deltaTime);
    void UpdateLookAtMatrix();
    Ray GetMouseRay(int windowWidth, int windowHeight);

    [[nodiscard]] glm::vec3 CalculateVelocity(glm::vec4 movement, float up, float down) const;
    std::vector<Vertex> GetTransformedVertices();
};


#endif //GJK_CAMERA_H
