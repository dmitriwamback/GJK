//
// Created by Dmitri on 2026-10-23.
//

#include "Cube.h"

#include <GL/glew.h>

#include "../../util/MathUtil.h"

Cube::Cube() {

    vertices = {
        Vertex({-1.0f, -1.0f, -1.0f},  {0, 0, -1}, {0, 0}),
        Vertex({ 1.0f,  1.0f, -1.0f},  {0, 0, -1}, {1, 1}),
        Vertex({ 1.0f, -1.0f, -1.0f},  {0, 0, -1}, {1, 0}),
        Vertex({ 1.0f,  1.0f, -1.0f},  {0, 0, -1}, {1, 1}),
        Vertex({-1.0f, -1.0f, -1.0f},  {0, 0, -1}, {0, 0}),
        Vertex({-1.0f,  1.0f, -1.0f},  {0, 0, -1}, {0, 1}),

        Vertex({-1.0f, -1.0f,  1.0f},  {0, 0, 1}, {0, 0}),
        Vertex({ 1.0f, -1.0f,  1.0f},  {0, 0, 1}, {1, 0}),
        Vertex({ 1.0f,  1.0f,  1.0f},  {0, 0, 1}, {1, 1}),
        Vertex({ 1.0f,  1.0f,  1.0f},  {0, 0, 1}, {1, 1}),
        Vertex({-1.0f,  1.0f,  1.0f},  {0, 0, 1}, {0, 1}),
        Vertex({-1.0f, -1.0f,  1.0f},  {0, 0, 1}, {0, 0}),

        Vertex({-1.0f,  1.0f,  1.0f},  {-1, 0, 0}, {1, 1}),
        Vertex({-1.0f,  1.0f, -1.0f},  {-1, 0, 0}, {1, 0}),
        Vertex({-1.0f, -1.0f, -1.0f},  {-1, 0, 0}, {0, 0}),
        Vertex({-1.0f, -1.0f, -1.0f},  {-1, 0, 0}, {0, 0}),
        Vertex({-1.0f, -1.0f,  1.0f},  {-1, 0, 0}, {0, 1}),
        Vertex({-1.0f,  1.0f,  1.0f},  {-1, 0, 0}, {1, 1}),

        Vertex({ 1.0f,  1.0f,  1.0f},  {1, 0, 0}, {1, 1}),
        Vertex({ 1.0f, -1.0f, -1.0f},  {1, 0, 0}, {0, 0}),
        Vertex({ 1.0f,  1.0f, -1.0f},  {1, 0, 0}, {1, 0}),
        Vertex({ 1.0f, -1.0f, -1.0f},  {1, 0, 0}, {0, 0}),
        Vertex({ 1.0f,  1.0f,  1.0f},  {1, 0, 0}, {1, 1}),
        Vertex({ 1.0f, -1.0f,  1.0f},  {1, 0, 0}, {0, 1}),

        Vertex({-1.0f, -1.0f, -1.0f},  {0, -1, 0}, {0, 0}),
        Vertex({ 1.0f, -1.0f, -1.0f},  {0, -1, 0}, {1, 0}),
        Vertex({ 1.0f, -1.0f,  1.0f},  {0, -1, 0}, {1, 1}),
        Vertex({ 1.0f, -1.0f,  1.0f},  {0, -1, 0}, {1, 1}),
        Vertex({-1.0f, -1.0f,  1.0f},  {0, -1, 0}, {0, 1}),
        Vertex({-1.0f, -1.0f, -1.0f},  {0, -1, 0}, {0, 0}),

        Vertex({-1.0f,  1.0f, -1.0f},  {0, 1, 0}, {0, 0}),
        Vertex({ 1.0f,  1.0f,  1.0f},  {0, 1, 0}, {1, 1}),
        Vertex({ 1.0f,  1.0f, -1.0f},  {0, 1, 0}, {1, 0}),
        Vertex({ 1.0f,  1.0f,  1.0f},  {0, 1, 0}, {1, 1}),
        Vertex({-1.0f,  1.0f, -1.0f},  {0, 1, 0}, {0, 0}),
        Vertex({-1.0f,  1.0f,  1.0f},  {0, 1, 0}, {0, 1}),
    };

    position = glm::vec3(0.0f, 0.0f, 0.0f);
    rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    scale = glm::vec3(1.0f, 1.0f, 1.0f);

    color = glm::vec3(1.0f);

    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);

    glGenBuffers(1, &vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
}

void Cube::Render(Shader shader) {

    shader.Use();
    glm::mat4 model = MathUtil::ComputeModelMatrix(position, rotation, scale);

    shader.SetMatrix4("model", model);
    shader.SetVector3("color", color);

    glBindVertexArray(vertexArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);

    glDrawArrays(GL_TRIANGLES, 0, 36);
}
