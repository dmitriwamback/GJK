//
//  object.h
//  GJK
//
//  Created by Dmitri Wamback on 2025-03-12.
//

#ifndef object_h
#define object_h

namespace core {

class RObject {
public:
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    
    uint32_t vao, vbo, ebo;
    
    glm::vec3 position, scale, rotation, color;
    
    virtual void Render(Shader shader, GLenum renderingType, bool identityMatrix) {}
    std::vector<Vertex> GetColliderVertices(bool withNormals);
    glm::mat4 CreateModelMatrix();
};

std::vector<Vertex> RObject::GetColliderVertices(bool withNormals = false) {
    
    glm::mat4 model = CreateModelMatrix();
    
    std::vector<Vertex> projectedVertices = std::vector<Vertex>();
    
    for (int i = 0; i < vertices.size(); i++) {
        glm::vec3 vertex = vertices[i].vertex;
        glm::vec3 projected = glm::vec3(model * glm::vec4(vertex, 1.0));
        projectedVertices.push_back(Vertex(projected, glm::vec3(0.0f), glm::vec2(0.0f)));
    }
    return projectedVertices;
}

glm::mat4 RObject::CreateModelMatrix() {
    
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

#endif /* object_h */
