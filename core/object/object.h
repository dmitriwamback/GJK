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
    std::vector<float> vertices;
    std::vector<uint32_t> indices;
    
    uint32_t vao, vbo, ebo;
    
    glm::vec3 position, scale, rotation, color;
    
    virtual void Render(Shader shader, GLenum renderingType, bool identityMatrix) {}
    std::vector<float> GetColliderVertices(bool withNormals);
    glm::mat4 CreateModelMatrix();
};

std::vector<float> RObject::GetColliderVertices(bool withNormals = false) {
    
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
