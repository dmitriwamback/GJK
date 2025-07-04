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
        
    std::vector<Vertex> vertices = {};
    std::vector<uint32_t> indices = {};
    
    int index = 0;
    srand(time(0));
    double seed = rand() % 10000 + 0.01f;
    int octaves = 16;
    
    float maxHeight = -FLT_MAX;
    float minHeight =  FLT_MAX;
    float offset = 0.01f;
    float currentHeight = 0.0f;
    
    glm::vec3 origin = glm::vec3(0.0f);
    
    for (int x = 0; x < terrain_size; x++) {
        for (int z = 0; z < terrain_size; z++) {
            
            index = z + x * terrain_size;
            
            float height = noiseLayer((float)x / terrain_size, (float)z / terrain_size, 2.2f, 0.5f, octaves, seed) * 1.5f;
            float heightR = noiseLayer((float)(x+1) / terrain_size, (float)z / terrain_size, 2.2f, 0.5f, octaves, seed) * 1.5f;
            float heightD = noiseLayer((float)x / terrain_size, (float)(z+1) / terrain_size, 2.2f, 0.5f, octaves, seed) * 1.5f;
            
            glm::vec3 p1 = glm::vec3(x - terrain_size / 2, height - 10.0f, z - terrain_size / 2);
            glm::vec3 p2 = glm::vec3((x + 1) - terrain_size / 2, heightR - 10.0f, z - terrain_size / 2);
            glm::vec3 p3 = glm::vec3(x - terrain_size / 2, heightD - 10.0f, (z + 1) - terrain_size / 2);
            
            glm::vec3 averagePoint = (p1 + p2 + p3)/glm::vec3(3.0f);
            float dist = glm::distance(glm::vec3(averagePoint.x, 0, averagePoint.z), origin);
            
            glm::vec3 normal = -CalculateNormalVector(p1, p2, p3);
            
            if (dist < 10.0f) {
                currentHeight = averagePoint.y;
            }
            
            if (height + offset > maxHeight) {
                maxHeight = height + offset;
            }
            if (heightR + offset > maxHeight) {
                maxHeight = heightR + offset;
            }
            if (heightD + offset > maxHeight) {
                maxHeight = heightD + offset;
            }
            
            if (height + offset < minHeight) {
                minHeight = height + offset;
            }
            if (heightR + offset < minHeight) {
                minHeight = heightR + offset;
            }
            if (heightD + offset < minHeight) {
                minHeight = heightD + offset;
            }
            
            glm::vec3 vertex = glm::vec3(p1.x, p1.y, p1.z);
            glm::vec3 _normal = glm::vec3(0.0f, 1.0f, 0.0f);
            glm::vec2 uv = glm::vec2(0.0f);
            
            vertices.push_back(Vertex(vertex, _normal, uv));
            
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
    
    float positiony = 0 - currentHeight + 500.0f;
    
    terrain->position = glm::vec3(0.0f, positiony, 0.0f);
    terrain->rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    terrain->scale = glm::vec3(0.4f * 10.0f, 2.0f * 10.0f, 0.4f * 10.0f);
    
    terrain->color = glm::vec3(1.0f);
    
    glGenVertexArrays(1, &terrain->vao);
    glBindVertexArray(terrain->vao);
    
    glGenBuffers(1, &terrain->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, terrain->vbo);
    glBufferData(GL_ARRAY_BUFFER, terrain->vertices.size() * sizeof(Vertex), &terrain->vertices[0], GL_STATIC_DRAW);
    
    glGenBuffers(1, &terrain->ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrain->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (terrain_size - 1) * (terrain_size - 1) * 6 * sizeof(uint32_t), &terrain->indices[0], GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, vertex));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
    
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
