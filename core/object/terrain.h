//
//  terrain.h
//  GJK
//
//  Created by Dmitri Wamback on 2025-03-12.
//

#ifndef terrain_h
#define terrain_h

#define terrain_size 500

namespace core {

class Terrain: public RObject {
public:
    
    static RObject* Create();
    void Render(Shader shader, GLenum renderingType, bool identityMatrix);
};

RObject* Terrain::Create() {
    RObject* terrain = new Terrain();
        
    std::vector<float> vertices = {};
    std::vector<uint32_t> indices = {};
    
    int index = 0;
    
    for (int x = 0; x < terrain_size; x++) {
        for (int z = 0; z < terrain_size; z++) {
            
            index = z + x * terrain_size;
                        
            float height = noiseLayer((float)x / terrain_size, (float)z / terrain_size, 2.2f, 0.5f, 32) * 1.5f;
            float heightR = noiseLayer((float)(x+1) / terrain_size, (float)z / terrain_size, 2.2f, 0.5f, 32) * 1.5f;
            float heightD = noiseLayer((float)x / terrain_size, (float)(z+1) / terrain_size, 2.2f, 0.5f, 32) * 1.5f;

            glm::vec3 p1 = glm::vec3(x - terrain_size / 2, height - 10.0f, z - terrain_size / 2);
            glm::vec3 p2 = glm::vec3((x + 1) - terrain_size / 2, heightR - 10.0f, z - terrain_size / 2);
            glm::vec3 p3 = glm::vec3(x - terrain_size / 2, heightD - 10.0f, (z + 1) - terrain_size / 2);

            glm::vec3 normal = -CalculateNormalVector(p1, p2, p3);

            vertices.push_back(p1.x);
            vertices.push_back(p1.y);
            vertices.push_back(p1.z);

            vertices.push_back(normal.x);
            vertices.push_back(normal.y);
            vertices.push_back(normal.z);
            
            if (x != terrain_size - 1 && z != terrain_size - 1) {
                indices.push_back(index);
                indices.push_back(index+1);
                indices.push_back(index+terrain_size);
                indices.push_back(index+1);
                indices.push_back(index+terrain_size+1);
                indices.push_back(index+terrain_size);
            }
        }
    }
    
    terrain->vertices = vertices;
    terrain->indices = indices;
    
    terrain->position = glm::vec3(0.0f, 0.0f, 0.0f);
    terrain->rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    terrain->scale = glm::vec3(0.4f, 2.0f, 0.4f);
    
    terrain->color = glm::vec3(1.0f);
    
    glGenVertexArrays(1, &terrain->vao);
    glBindVertexArray(terrain->vao);
    
    glGenBuffers(1, &terrain->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, terrain->vbo);
    //throw std::runtime_error(std::to_string(vertices.size()));
    glBufferData(GL_ARRAY_BUFFER, terrain_size * terrain_size * 6 * sizeof(float), &terrain->vertices[0], GL_STATIC_DRAW);
    
    glGenBuffers(1, &terrain->ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrain->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (terrain_size - 1) * (terrain_size - 1) * 6 * sizeof(uint32_t), &terrain->indices[0], GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    
    return terrain;
}

void Terrain::Render(Shader shader, GLenum renderingType, bool identityMatrix) {
    
    shader.Use();
    
    glm::mat4 model = CreateModelMatrix();
        
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    
    shader.SetMatrix4("model", model);
    shader.SetVector3("color", color);
    
    glDrawElements(renderingType, (terrain_size - 1) * (terrain_size - 1) * 6, GL_UNSIGNED_INT, nullptr);
    
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
}


}

#endif /* terrain_h */
