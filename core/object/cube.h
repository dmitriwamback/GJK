//
//  cube.h
//  GJK
//
//  Created by Dmitri Wamback on 2025-01-02.
//

#ifndef cube_h
#define cube_h

namespace core {

class Cube: public RObject {
public:
    static RObject* Create();
    void Render(Shader shader, GLenum renderingType, bool identityMatrix);
};

RObject* Cube::Create() {
    RObject* cube = new Cube();
        
    std::vector<Vertex> vertices = {
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
    
    cube->vertices = vertices;
    cube->indices = std::vector<uint32_t>();
    
    cube->position = glm::vec3(0.0f, 0.0f, 0.0f);
    cube->rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    cube->scale = glm::vec3(1.0f, 1.0f, 1.0f);
    
    cube->color = glm::vec3(1.0f);
    
    glGenVertexArrays(1, &cube->vao);
    glBindVertexArray(cube->vao);
    
    glGenBuffers(1, &cube->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, cube->vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), cube->vertices.data(), GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, vertex));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
    
    return cube;
}

void Cube::Render(Shader shader, GLenum renderingType, bool identityMatrix) {
    
    shader.Use();
    
    glm::mat4 model = CreateModelMatrix();
        
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
    
    if (identityMatrix) {
        std::vector<Vertex> projectedVertices = GetColliderVertices(true);
        model = glm::mat4(1.0f);
        
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, projectedVertices.size() * sizeof(Vertex), projectedVertices.data(), GL_STATIC_DRAW);
    }
    
    shader.SetMatrix4("model", model);
    shader.SetVector3("color", color);
    
    glDrawArrays(renderingType, 0, 36);
}

}

#endif /* cube_h */
