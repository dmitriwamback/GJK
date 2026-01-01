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
float deltaTime = 0;
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

std::vector<core::Triangle> BuildTrianglesFromRObject(RObject* obj) {
    std::vector<core::Triangle> tris;

    std::vector<Vertex> verts = obj->GetColliderVertices();
    const std::vector<uint32_t>& indices = obj->indices;

    if (!indices.empty()) {
        for (size_t i = 0; i < indices.size(); i += 3) {
            const glm::vec3& a = verts[indices[i]].vertex;
            const glm::vec3& b = verts[indices[i + 1]].vertex;
            const glm::vec3& c = verts[indices[i + 2]].vertex;
            tris.emplace_back(a, b, c);
        }
    }

    else {
        for (size_t i = 0; i < verts.size(); i += 3) {
            const glm::vec3& a = verts[i].vertex;
            const glm::vec3& b = verts[i + 1].vertex;
            const glm::vec3& c = verts[i + 2].vertex;
            tris.emplace_back(a, b, c);
        }
    }

    return tris;
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
    RObject *mouseRayCube = Cube::Create(), *debugRaycastCube = Cube::Create(), *terrain = Terrain::Create();
    std::vector<RObject*> colliderCubes;
    
    core::OctreeNode* rootOctree = new core::OctreeNode();
    rootOctree->min = glm::vec3(-500.0f, -500.0f, -500.0f);
    rootOctree->max = glm::vec3(500.0f, 500.0f, 500.0f);
    
    for (int i = -10; i < 10; i++) {
        for (int j = -10; j < 10; j++) {
            RObject *newCube = Cube::Create();
            newCube->scale = glm::vec3(rand()%5 + 0.5f, rand()%5 + 0.5f, rand()%5 + 0.5f);
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
    
    debugRaycastCube->scale = glm::vec3(10.0f, 1.0f, 12.0f);
    debugRaycastCube->rotation = glm::vec3(45.0f, 0.0f, 0.0f);
    debugRaycastCube->position = glm::vec3(0.0f, 0.0f, -40.0f);
    debugRaycastCube->color = glm::vec3(0.8f);
    
    shader = Shader::Create("/Users/dmitriwamback/Documents/Projects/GJK/GJK/shader/main");

    double lastFrameTime = glfwGetTime();
    double fpsTimer = lastFrameTime;
    int frameCount = 0;

    while (!glfwWindowShouldClose(window)) {

        double currentTime = glfwGetTime();
        core::deltaTime = float(currentTime - lastFrameTime);
        lastFrameTime = currentTime;

        frameCount++;
        if (currentTime - fpsTimer >= 1.0) {
            std::stringstream ss;
            ss << "GJK Algorithm - FPS: " << frameCount;
            glfwSetWindowTitle(window, ss.str().c_str());
            frameCount = 0;
            fpsTimer += 1.0;
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.3f, 0.3f, 0.3f, 0.0f);

        glm::vec4 movement = glm::vec4(0.0f);
        movement.z = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS ?  1.0f : 0.0f;
        movement.w = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS ? -1.0f : 0.0f;
        movement.x = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS ?  1.0f : 0.0f;
        movement.y = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS ? -1.0f : 0.0f;
        
        float up   = glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS ?  1.0f : 0.0f;
        float down = glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS ? -1.0f : 0.0f;
                        
        scroll = camera.lastYScroll;
        if (scroll < 5.0f) scroll = 5.0f;
        
        mouseRayCube->position = camera.mouseRayDirection * 10.0f + camera.position;
        mouseRayCube->color = glm::vec3(0.8f);
        debugRaycastCube->color = glm::vec3(0.8f);
        terrain->color = glm::vec3(0.8f);

        Ray ray{};
        ray.origin = camera.position;
        ray.direction = camera.mouseRayDirection;
        
        glm::vec3 futureStep = camera.Step(movement, up, down), futureStep2 = camera.Step(movement, up, down, 2.0f);
        glm::vec3 differentInSteps = futureStep - camera.position;
        
        std::cout << "-----------------------------------------------------------------------------" << '\n';
        
        std::cout << "Future Step: ";
        std::cout << futureStep.x << "x " << futureStep.y << "y " << futureStep.z << 'z' <<  '\n';
        
        std::cout << "Current Position: ";
        std::cout << camera.position.x << "x " << camera.position.y << "y " << camera.position.z << 'z' << '\n';
        
        std::cout << "Difference: ";
        std::cout << differentInSteps.x << "x " << differentInSteps.y << "y " << differentInSteps.z << 'z' << '\n';
        
        std::cout << "-----------------------------------------------------------------------------" << '\n';
        
        std::cout << "DeltaTime: " << core::deltaTime << "s\n";
        
        camera.Update(movement, up, down);

        std::vector<RObject*> candidates;
        glm::vec3 queryMin = camera.position - glm::vec3(camera.speed * 1.5f) * 0.5f;
        glm::vec3 queryMax = camera.position + glm::vec3(camera.speed * 1.5f) * 0.5f;

        if (rootOctree->children[0]) {
            candidates = core::ParallelQuery(rootOctree, queryMin, queryMax);
        }
        else {
            core::QueryObjects(rootOctree, queryMin, queryMax, candidates);
        }
        
        std::optional<Intersection> intersect = Raycast(Ray{camera.position, camera.mouseRayDirection}, BuildTrianglesFromRObject(debugRaycastCube));
        
        if (intersect) {
            debugRaycastCube->color = glm::vec3(0.0f, 0.0f, 0.9f);
            mouseRayCube->position = intersect->intersectionPoint;
            collision col = GJKCollision(mouseRayCube, debugRaycastCube);
            
            if (col.collided) {
                if (glm::dot(col.normal, mouseRayCube->position - debugRaycastCube->position) < 0) col.normal = -col.normal;
                mouseRayCube->position += col.normal * col.depth;
            }
        }
        else {
            debugRaycastCube->color = glm::vec3(0.8f);
        }

        for (RObject *_cube : candidates) {
            collision col = GJKCollision(_cube, mouseRayCube);
            collision cameraCol = GJKCollisionWithCamera(_cube);
            
            bool collidedWithCube = false;

            if (col.collided) {
                collidedWithCube = true;
                if (glm::dot(col.normal, mouseRayCube->position - _cube->position) < 0) col.normal = -col.normal;

                mouseRayCube->position += col.normal * col.depth;
                mouseRayCube->color = glm::vec3(0.9f, 0.0f, 0.0f);
            }
            if (cameraCol.collided) {
                collidedWithCube = true;
                if (glm::dot(cameraCol.normal, camera.position - _cube->position) < 0) cameraCol.normal = -cameraCol.normal;

                camera.position += cameraCol.normal * cameraCol.depth;
            }
            
            if (collidedWithCube) {
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

        for (RObject *_cube : colliderCubes) renderDebugCube(_cube);
        renderDebugCube(mouseRayCube);
        renderDebugCube(debugRaycastCube);
        renderDebugCube(terrain);

        t += 0.01f;

        glfwPollEvents();
        glfwSwapBuffers(window);
    }
}

}


#endif /* gjkmain_h */
