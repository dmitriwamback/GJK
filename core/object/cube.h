//
//  cube.h
//  GJK
//
//  Created by Dmitri Wamback on 2025-01-02.
//

#ifndef cube_h
#define cube_h

namespace core {

class Cube {
public:
    std::vector<float> vertices;
    std::vector<uint32_t> indices;
    
    uint32_t vao, vbo, ibo;
    
    glm::vec3 position, scale, rotation, color;
    
    static Cube Create();
    void Render(Shader shader, GLenum renderingType, bool identityMatrix);
    std::vector<float> GetColliderVertices(bool withNormals);
    glm::mat4 CreateModelMatrix();
};

Cube Cube::Create() {
    Cube cube = Cube();
        
    std::vector<float> vertices = {
        -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, -1.0f,
         1.0f,  1.0f,  1.0f,  0.0f,  0.0f, -1.0f,
         1.0f, -1.0f,  1.0f,  0.0f,  0.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         1.0f,  1.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         1.0f, -1.0f, -1.0f,  0.0f,  0.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,  0.0f,  0.0f,  1.0f,
    };
    std::vector<uint32_t> indices = {
        0, 2, 1, 0, 3, 2,  // Reversed (CW)

        // Back face
        5, 7, 4, 5, 6, 7,  // Reversed (CW)

        // Left face
        4, 3, 0, 4, 7, 3,  // Reversed (CW)

        // Right face
        1, 6, 5, 1, 2, 6,  // Reversed (CW)

        // Top face
        4, 1, 5, 4, 0, 1,  // Reversed (CW)

        // Bottom face
        3, 6, 2, 3, 7, 6   // Reversed (CW)
    };
    
    cube.vertices = vertices;
    cube.indices = indices;
    
    cube.position = glm::vec3(0.0f, 0.0f, 0.0f);
    cube.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    cube.scale = glm::vec3(1.0f, 1.0f, 1.0f);
    
    cube.color = glm::vec3(1.0f);
    
    glGenVertexArrays(1, &cube.vao);
    glBindVertexArray(cube.vao);
    
    glGenBuffers(1, &cube.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, cube.vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &cube.vertices[0], GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    
    glGenBuffers(1, &cube.ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube.ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), &cube.indices[0], GL_STATIC_DRAW);
    
    return cube;
}

void Cube::Render(Shader shader, GLenum renderingType = GL_TRIANGLES, bool identityMatrix = false) {
    
    shader.Use();
    
    glm::mat4 model = CreateModelMatrix();
        
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), &indices[0], GL_STATIC_DRAW);
    
    if (identityMatrix) {
        std::vector<float> projectedVertices = GetColliderVertices(true);
        
        model = glm::mat4(1.0f);
        
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, projectedVertices.size() * sizeof(float), &projectedVertices[0], GL_STATIC_DRAW);
    }
    
    shader.SetMatrix4("model", model);
    shader.SetVector3("color", color);
    
    glDrawElements(renderingType, indices.size(), GL_UNSIGNED_INT, nullptr);
}

std::vector<float> Cube::GetColliderVertices(bool withNormals = false) {
    
    glm::mat4 model = CreateModelMatrix();
    
    std::vector<float> projectedVertices = std::vector<float>();
    
    for (int i = 0; i < vertices.size()/6; i++) {
        glm::vec3 vertex = glm::vec3(vertices[i * 6], vertices[i * 6 + 1], vertices[i * 6 + 2]);
        glm::vec3 projected = glm::vec3(model * glm::vec4(vertex, 1.0));
        projectedVertices.push_back(projected.x);
        projectedVertices.push_back(projected.y);
        projectedVertices.push_back(projected.z);
        if (withNormals) {
            projectedVertices.push_back(0);
            projectedVertices.push_back(0);
            projectedVertices.push_back(0);
        }
    }
    return projectedVertices;
}

glm::mat4 Cube::CreateModelMatrix() {
    
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 translationMatrix = glm::mat4(1.0f);
    translationMatrix = glm::translate(translationMatrix, position);
    
    glm::mat4 scaleMatrix = glm::mat4(1.0f);
    scaleMatrix = glm::scale(scaleMatrix, scale);
    
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x), glm::vec3(1, 0, 0)) *
                               glm::rotate(glm::mat4(1.0f), glm::radians(rotation.y), glm::vec3(0, 1, 0)) *
                               glm::rotate(glm::mat4(1.0f), glm::radians(rotation.z), glm::vec3(0, 0, 1));
    
    model = translationMatrix * rotationMatrix * scaleMatrix;
    
    return model;
}

}

#endif /* cube_h */
