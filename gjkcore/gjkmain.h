//
//  gjkmain.h
//  GJK
//
//  Created by Dmitri Wamback on 2024-12-30.
//

#ifndef gjkmain_h
#define gjkmain_h

#include "object/camera.h"

namespace gjk {

GLFWwindow* window;

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
    
    while (!glfwWindowShouldClose(window)) {
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.3, 0.3, 0.3, 1.0);
        
        glfwPollEvents();
        glfwSwapBuffers(window);
    }
}

}


#endif /* gjkmain_h */
