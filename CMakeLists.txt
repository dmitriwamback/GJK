cmake_minimum_required(VERSION 4.3)
project(GJK)

set(CMAKE_CXX_STANDARD 20)

find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)

add_executable(GJK main.cpp
        collision/GJK.cpp
        collision/GJK.h
        collision/EPA.cpp
        collision/EPA.cpp
        collision/EPA.h
        renderer/Renderer.cpp
        renderer/Renderer.h
        renderer/Scene.cpp
        renderer/Scene.h
        renderer/objects/Vertex.h
        renderer/objects/Cube.cpp
        renderer/objects/Cube.h
        renderer/objects/GameObject.cpp
        renderer/objects/GameObject.h
        util/MathUtil.cpp
        util/MathUtil.h
        util/OctreeNode.cpp
        util/OctreeNode.h
        renderer/camera/Camera.cpp
        renderer/camera/Camera.h
        renderer/camera/Raycast.cpp
        renderer/camera/Raycast.h
        renderer/Shader.cpp
        renderer/Shader.h
        renderer/ShaderSources.h
        collision/Simplex.h
        collision/Support.h
        collision/Simplex.cpp
        collision/Simplex.h)

target_link_libraries(GJK PRIVATE
        glfw
        GLEW::GLEW
        glm::glm
)