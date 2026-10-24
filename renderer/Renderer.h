//
// Created by Dmitri on 2026-10-23.
//

#ifndef GJK_RENDERER_H
#define GJK_RENDERER_H

#include <GL/glew.h>
#include <glfw/glfw3.h>

#include "Scene.h"
#include "../util/OctreeNode.h"
#include "camera/Camera.h"

class Renderer {
public:
    void Initialize();
    void GameLoop();

private:
    std::unique_ptr<Scene> scene;
    Camera camera;

    OctreeNode* rootNode;
    GLFWwindow *window;

    static void CursorPositionCallback(GLFWwindow *window, double xpos, double ypos);
};


#endif //GJK_RENDERER_H
