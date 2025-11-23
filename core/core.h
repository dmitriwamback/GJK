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

#include "object/vertex.h"

#include "object/object.h"
#include "object/camera.h"
#include "object/cube.h"

#include "math/raycast.h"
#include "math/simplex.h"
#include "math/support.h"
#include "math/epa.h"
#include "math/gjk.h"

#include "object/octree_node.h"

#include "math/noise.h"
#include "math/calculate_normal.h"
#include "object/terrain.h"

namespace core {

void renderDebugCube(RObject* object) {
    
    object->Render(shader, GL_TRIANGLES, false);
    
    //object->color = glm::vec3(0.0f);
    //object->Render(shader, GL_LINES, true);
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
    RObject *mouseRayCube = Cube::Create(), *cube3 = Cube::Create(), *terrain = Terrain::Create();
    std::vector<RObject*> colliderCubes;
    
    core::OctreeNode* rootOctree = new core::OctreeNode();
    rootOctree->min = glm::vec3(-500.0f, -500.0f, -500.0f);
    rootOctree->max = glm::vec3(500.0f, 500.0f, 500.0f);
    
    for (int i = -20; i < 20; i++) {
        for (int j = -20; j < 20; j++) {
            RObject *newCube = Cube::Create();
            newCube->scale = glm::vec3(3.0f, 3.0f, 3.0f);
            newCube->rotation = glm::vec3(rand()%360, rand()%360, rand()%360);
            newCube->position = glm::vec3(i * 10, -1.0f, j * 10);
            newCube->color = glm::vec3(0.8f);
            colliderCubes.push_back(newCube);
        }
    }
    
    for (RObject* cube : colliderCubes) {
        core::InsertObject(rootOctree, cube);
    }
    
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetScrollCallback(window, scroll_callback);
    
    float t = 0.0f;
    float scroll = 10.0f;
    
    mouseRayCube->rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    mouseRayCube->position = glm::vec3(0.0f, 10.0f, 0.0f);
    
    cube3->scale = glm::vec3(10.0f, 1.0f, 12.0f);
    cube3->rotation = glm::vec3(45.0f, 0.0f, 0.0f);
    cube3->position = glm::vec3(0.0f, 0.0f, -40.0f);
    cube3->color = glm::vec3(0.8f);
    
    shader = Shader::Create("/Users/dmitriwamback/Documents/Projects/GJK/GJK/shader/main");
    
    double previousTime = glfwGetTime();
    int frameCount = 0;
    
    while (!glfwWindowShouldClose(window)) {
        
        double currentTime = glfwGetTime();
        frameCount++;

        if (currentTime - previousTime >= 1.0) {
            std::stringstream ss;
            ss << "GJK Algorithm - FPS: " << frameCount;
            glfwSetWindowTitle(window, ss.str().c_str());
            
            frameCount = 0;
            previousTime = currentTime;
        }
        
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
        
        mouseRayCube->position = camera.mouseRayDirection * 10.0f + camera.position;
        mouseRayCube->color = glm::vec3(0.8f);
        
        cube3->color = glm::vec3(0.8f);
        
        terrain->color = glm::vec3(0.8f);
        
        Ray ray{};
        ray.origin = camera.position;
        ray.direction = camera.mouseRayDirection;
        std::optional<Intersection> intersect = Raycast(ray, cube3->GetColliderVertices(), cube3->indices);
        if (intersect) {
            cube3->color = glm::vec3(0.0f, 0.0f, 0.9f);
            mouseRayCube->position = intersect->intersectionPoint;
        }
        
        camera.Update(movement, up, down);
        
        std::vector<RObject*> candidates;
        glm::vec3 queryMin = camera.position - glm::vec3(20.0f) * 0.5f;
        glm::vec3 queryMax = camera.position + glm::vec3(20.0f) * 0.5f;

        if (rootOctree->children[0]) {
            candidates = core::ParallelQuery(rootOctree, queryMin, queryMax);
        }
        else {
            core::QueryObjects(rootOctree, queryMin, queryMax, candidates);
        }
        
        for (RObject *_cube : candidates) {
            
            collision col = GJKCollision(_cube, mouseRayCube),
                      cameraCol = GJKCollisionWithCamera(_cube);
            if (col.collided) {
                
                if (glm::dot(col.normal, mouseRayCube->position - _cube->position) < 0) {
                    col.normal = -col.normal;
                }
                                
                _cube->color = glm::vec3(0.9f, 0.0f, 0.0f);
                mouseRayCube->position += col.normal*col.depth;
                
                mouseRayCube->color = glm::vec3(0.9f, 0.0f, 0.0f);
            }
            
            if (cameraCol.collided) {
                
                if (glm::dot(cameraCol.normal, _cube->position - camera.position) > 0) {
                    cameraCol.normal = -cameraCol.normal;
                }
                camera.position += cameraCol.normal*cameraCol.depth;
                
                _cube->color = glm::vec3(0.9f, 0.0f, 0.0f);
            }
            else {
                _cube->color = glm::vec3(0.8f);
            }
        }
        
        camera.UpdateLookAtMatrix();
        
        shader.Use();
        shader.SetMatrix4("projection", camera.projection);
        shader.SetMatrix4("lookAt", camera.lookAt);
        
        for (RObject *_cube : colliderCubes) {
            renderDebugCube(_cube);
        }
        renderDebugCube(mouseRayCube);
        renderDebugCube(cube3);
        renderDebugCube(terrain);
        
        t += 0.01f;
        
        glfwPollEvents();
        glfwSwapBuffers(window);
    }
}

}


#endif /* gjkmain_h */
