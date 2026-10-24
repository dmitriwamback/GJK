//
// Created by Dmitri on 2026-10-23.
//

#ifndef GJK_GAMEOBJECT_H
#define GJK_GAMEOBJECT_H
#include <cstdint>
#include <vector>

#include "Vertex.h"
#include "../Shader.h"
#include "../camera/Raycast.h"


class GameObject {
public:
    virtual ~GameObject() = default;
    GameObject() = default;

    glm::vec3 position, rotation, scale, color;

    std::vector<Vertex> GetTransformedVertices();
    std::vector<Triangle> BuildTriangleMesh();
    virtual void Render(Shader shader);
    std::optional<Intersection> Intercept(Ray ray);

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    uint32_t vertexArrayObject = 0, vertexBufferObject = 0, indexBufferObject = 0;
};


#endif //GJK_GAMEOBJECT_H
