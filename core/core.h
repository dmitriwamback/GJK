//
//  gjkmain.h
//  GJK
//
//  Created by Dmitri Wamback on 2024-12-30.
//

#ifndef gjkmain_h
#define gjkmain_h

#include <fstream>
#include <sstream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace gjk {
GLFWwindow* window;
}
#include "object/render/shader.h"
#include "object/camera.h"
#include "object/cube.h"

#include "gjk/gjk.h"

namespace gjk {


void initialize() {
    
    if (!glfwInit()) {
        throw std::runtime_error("Couldn't initialize glfw");
    }
    
#if defined(__APPLE__)
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    
    window = glfwCreateWindow(1200, 800, "GJK Algorithm", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    
    glewExperimental = GL_TRUE;
    glewInit();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_PROGRAM_POINT_SIZE);
    
    gjk::core::Camera::Initialize();
    gjk::core::Shader shader = gjk::core::Shader::Create("/Users/dmitriwamback/Documents/Projects/GJK/GJK/shader/main");
    gjk::core::Cube cube = gjk::core::Cube::Create();
    glfwSetCursorPosCallback(window, gjk::core::cursor_position_callback);
    
    while (!glfwWindowShouldClose(window)) {
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.3, 0.3, 0.3, 0.0);
        
        glm::vec4 movement = glm::vec4(0.0f);

        movement.z = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS ?  0.05f : 0;
        movement.w = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS ? -0.05f : 0;
        movement.x = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS ?  0.05f : 0;
        movement.y = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS ? -0.05f : 0;
        
        float up = glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS ? 0.05f : 0;
        float down = glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS ? -0.05f : 0;
        
        gjk::core::camera.Update(movement, up, down);
        
        shader.Use();
        shader.SetMatrix4("projection", gjk::core::camera.projection);
        shader.SetMatrix4("lookAt", gjk::core::camera.lookAt);
        
        if (gjk::core::math::CollideWithCamera(cube)) {
            cube.color = glm::vec3(0.0f, 1.0f, 0.0f);
        }
        else {
            cube.color = glm::vec3(1.0f);
        }
        cube.Render(shader);
        
        
        cube.color = glm::vec3(1.0f, 0.0f, 0.0f);
        cube.Render(shader, GL_POINTS, true);
        cube.Render(shader, GL_LINES, true);
        
        glfwPollEvents();
        glfwSwapBuffers(window);
    }
}

}


#endif /* gjkmain_h */
