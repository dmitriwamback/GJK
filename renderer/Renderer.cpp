//
// Created by Dmitri on 2026-10-23.
//

#include "Renderer.h"

#include <iostream>
#include <ostream>
#include <sstream>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../util/MathUtil.h"
#include "objects/GameObject.h"
#include "objects/Cube.h"

void Renderer::Initialize() {

    if (!glfwInit()) {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return;
    }

#if defined(__APPLE__)
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    window = glfwCreateWindow(1200, 800, "GJK Algorithm", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    glfwSetWindowUserPointer(window, this);
    glfwSetCursorPosCallback(window, CursorPositionCallback);

    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "GLEW init failed: " << glewGetErrorString(err) << std::endl;
    }
    glEnable(GL_DEPTH_TEST);

    camera = Camera();
    scene = std::make_unique<Scene>();

    rootNode = new OctreeNode();
    rootNode->min = glm::vec3(-500.0f, -500.0f, -500.0f);
    rootNode->max = glm::vec3(500.0f, 500.0f, 500.0f);

    for (int x = -15; x <= 15; x++) {
        for (int z = -15; z <= 15; z++) {
            GameObject *gameObject = new Cube();
            gameObject->position = glm::vec3(x * 15, 0, z * 15);
            gameObject->rotation = glm::vec3(rand()%5, rand()%5, rand()%5);
            gameObject->scale = glm::vec3(rand()%4 + 1, rand()%4 + 1, rand()%4 + 1);
            gameObject->color = glm::vec3(1.0f);

            scene->Populate(rootNode, gameObject);
        }
    }
}

void Renderer::GameLoop() {

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

        double currentTime = glfwGetTime();
        deltaTime = float(currentTime - lastFrameTime);
        lastFrameTime = currentTime;

        frameCount++;
        if (currentTime - fpsTimer >= 1.0) {
            std::stringstream ss;
            ss << "GJK Algorithm - FPS: " << frameCount;
            glfwSetWindowTitle(window, ss.str().c_str());
            frameCount = 0;
            fpsTimer += 1.0;
        }

        glm::vec4 movement;
        movement.x = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS ?  1.0f : 0.0f;
        movement.y = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS ? -1.0f : 0.0f;
        movement.z = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS ?  1.0f : 0.0f;
        movement.w = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS ? -1.0f : 0.0f;

        float up   = glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS ?  1.0f : 0.0f;
        float down = glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS ? -1.0f : 0.0f;

        int windowWidth, windowHeight;
        glfwGetWindowSize(window, &windowWidth, &windowHeight);

        Ray cameraRay = camera.GetMouseRay(windowWidth, windowHeight);
        scene->CastRay(cameraRay);

        camera.Update(movement, up, down, window, deltaTime);
        scene->Collide(rootNode, camera);
        camera.UpdateLookAtMatrix();
        scene->Render(camera);

        glfwPollEvents();
        glfwSwapBuffers(window);
    }
}

void Renderer::CursorPositionCallback(GLFWwindow *window, double xpos, double ypos) {

    auto* self = static_cast<Renderer*>(glfwGetWindowUserPointer(window));

    if (glfwGetMouseButton(window, self->camera.mouseButton)) {
        float dy = ypos - self->camera.lastMouseY;
        float dx = xpos - self->camera.lastMouseX;

        self->camera.pitch -= dy * 0.005f;
        self->camera.yaw += dx * 0.005f;

        if (self->camera.pitch > 1.55f)  self->camera.pitch = 1.55f;
        if (self->camera.pitch < -1.55f) self->camera.pitch = -1.55f;

        self->camera.lookDirection = glm::normalize(glm::vec3(cos(self->camera.yaw) * cos(self->camera.pitch), sin(self->camera.pitch), sin(self->camera.yaw) * cos(self->camera.pitch)));
    }

    self->camera.lastMouseX = xpos;
    self->camera.lastMouseY = ypos;
}