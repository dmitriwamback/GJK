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

#include "object/object.h"
#include "object/camera.h"
#include "object/cube.h"

#include "math/raycast.h"
#include "math/simplex.h"
#include "math/support.h"
#include "math/epa.h"
#include "math/gjk.h"

#include "math/noise.h"
#include "math/calculate_normal.h"
#include "object/terrain.h"

namespace core {

void renderDebugCube(RObject* object) {
    
    object->Render(shader, GL_TRIANGLES, false);
    
    object->color = glm::vec3(0.0f);
    object->Render(shader, GL_LINES, true);
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
    RObject *cube = Cube::Create(), *cube2 = Cube::Create(), *cube3 = Cube::Create(), *terrain = Terrain::Create();
    
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetScrollCallback(window, scroll_callback);
    
    float t = 0.0f;
    float scroll = 10.0f;
    
    cube->scale = glm::vec3(10.0f, 10.0f, 10.0f);
    cube->rotation = glm::vec3(45.0f, 0.0f, 135.0f);
    cube->position = glm::vec3(1.0f, -1.0f, 0.0f);
    cube->color = glm::vec3(0.8f);
    
    cube2->rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    cube2->position = glm::vec3(0.0f, 10.0f, 0.0f);
    
    cube3->scale = glm::vec3(10.0f, 1.0f, 12.0f);
    cube3->rotation = glm::vec3(45.0f, 0.0f, 0.0f);
    cube3->position = glm::vec3(0.0f, 0.0f, -40.0f);
    cube3->color = glm::vec3(0.8f);
    
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
        
        std::cout << camera.position.x << " " << camera.position.y << " " << camera.position.z << '\n';
                        
        scroll = camera.lastYScroll;
        if (scroll < 5.0f) scroll = 5.0f;
        
        cube2->position = camera.mouseRayDirection * 10.0f + camera.position;
        cube2->color = glm::vec3(0.8f);
        
        cube->rotation.y += 0.1f;
        
        cube->color = glm::vec3(0.8f);
        
        cube3->color = glm::vec3(0.8f);
        
        terrain->color = glm::vec3(0.8f);
        
        Ray ray{};
        ray.origin = camera.position;
        ray.direction = camera.mouseRayDirection;
        std::optional<Intersection> intersect = Raycast(ray, cube3->GetColliderVertices(), cube3->indices);
        if (intersect) {
            cube3->color = glm::vec3(0.0f, 0.0f, 0.9f);
            cube2->position = intersect->intersectionPoint;
        }
        
        collision col = GJKCollision(cube, cube2);
        if (col.collided) {
            
            if (glm::dot(col.normal, cube2->position - cube->position) < 0) {
                col.normal = -col.normal;
            }
            
            std::cout << col.depth << '\n';
            
            cube->color = glm::vec3(0.9f, 0.0f, 0.0f);
            cube2->position += col.normal*col.depth;
            
            cube2->color = glm::vec3(0.9f, 0.0f, 0.0f);
        }
        
        collision cameraCol = GJKCollisionWithCamera(cube);
        if (cameraCol.collided) {
            
            if (glm::dot(cameraCol.normal, cube->position - camera.position) > 0) {
                cameraCol.normal = -cameraCol.normal;
            }
            camera.position += cameraCol.normal*cameraCol.depth;
            
            cube->color = glm::vec3(0.9f, 0.0f, 0.0f);
        }
        
        camera.Update(movement, up, down);
        
        shader.Use();
        shader.SetMatrix4("projection", camera.projection);
        shader.SetMatrix4("lookAt", camera.lookAt);
        
        renderDebugCube(cube);
        renderDebugCube(cube2);
        renderDebugCube(cube3);
        renderDebugCube(terrain);
        
        t += 0.01f;
        
        glfwPollEvents();
        glfwSwapBuffers(window);
    }
}

}


#endif /* gjkmain_h */
