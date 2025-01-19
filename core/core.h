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


#include "object/render/shader.h"

namespace core {
GLFWwindow* window;
Shader shader;
}

#include "object/camera.h"
#include "object/cube.h"

#include "math/raycast.h"
#include "math/simplex.h"
#include "math/gjk.h"
#include "math/epa.h"

namespace core {

void renderDebugCube(Cube cube) {
    
    cube.Render(shader);
    
    cube.color = glm::vec3(0.0f);
    cube.Render(shader, GL_LINES, true);
}

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
    
    Camera::Initialize();
    Cube cube = Cube::Create(), cube2 = Cube::Create(), cube3 = Cube::Create();
    
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetScrollCallback(window, scroll_callback);
    
    float t = 0.0f;
    float scroll = 10.0f;
    
    cube.scale = glm::vec3(10.0f, 1.0f, 1.0f);
    cube.rotation = glm::vec3(45.0f, 0.0f, 0.0f);
    cube.position = glm::vec3(1.0f, 0.0f, 0.0f);
    cube.color = glm::vec3(0.8f);
    
    cube3.scale = glm::vec3(10.0f, 1.0f, 12.0f);
    cube3.rotation = glm::vec3(45.0f, 0.0f, 0.0f);
    cube3.position = glm::vec3(0.0f, 0.0f, -40.0f);
    cube3.color = glm::vec3(0.8f);
    
    shader = Shader::Create("/Users/dmitriwamback/Documents/Projects/GJK/GJK/shader/main");
    
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
        
        camera.Update(movement, up, down);
                        
        scroll += camera.deltaScroll / 5.0f;
        if (scroll < 5.0f) scroll = 5.0f;
        
        cube2.position = camera.mouseRayDirection * 10.0f + camera.position;
        cube2.color = glm::vec3(0.8f);
        
        cube.rotation = glm::vec3(t * 10.0f, 0.0f, 45.0f + t * 10.0f);
        cube.color = glm::vec3(0.8f);
        
        cube3.color = glm::vec3(0.8f);
        
        if (GJKCollision(cube, cube2)) {
            cube.color = glm::vec3(0.9f, 0.0f, 0.0f);
            cube2.color = glm::vec3(0.9f, 0.0f, 0.0f);
        }
        if (GJKCollisionWithCamera(cube)) {
            cube.color = glm::vec3(0.9f, 0.0f, 0.0f);
        }
        
        Ray ray{};
        ray.origin = camera.position;
        ray.direction = camera.mouseRayDirection;
        std::optional<Intersection> intersect = Raycast(ray, cube3.GetColliderVertices(), cube3.indices);
        if (intersect) {
            cube3.color = glm::vec3(0.0f, 0.0f, 0.9f);
        }
        
        shader.Use();
        shader.SetMatrix4("projection", camera.projection);
        shader.SetMatrix4("lookAt", camera.lookAt);
        
        renderDebugCube(cube);
        renderDebugCube(cube2);
        renderDebugCube(cube3);
        
        t += 0.01f;
        
        glfwPollEvents();
        glfwSwapBuffers(window);
    }
}

}


#endif /* gjkmain_h */
