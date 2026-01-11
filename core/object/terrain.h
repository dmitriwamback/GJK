//
//  terrain.h
//  GJK
//
//  Created by Dmitri Wamback on 2025-03-12.
//

#ifndef terrain_h
#define terrain_h

#define terrain_size 129
#define chunk_quads 4

namespace core {

class ConvexCollider: public RObject {
public:
    glm::vec3 aabb_max, aabb_min;
};

class Terrain: public RObject {
public:
    std::vector<ConvexCollider*> colliders;

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
    
    for (int x = 0; x < terrain_size - 1; x++) {
        for (int z = 0; z < terrain_size - 1; z++) {
            
            /*
            float h00 = noiseLayer((float)x / terrain_size, (float)z / terrain_size, 2.2f, 0.5f, octaves, seed) * 1.5f;
            float h10 = noiseLayer((float)(x+1) / terrain_size, (float)z / terrain_size, 2.2f, 0.5f, octaves, seed) * 1.5f;
            float h01 = noiseLayer((float)x / terrain_size, (float)(z+1) / terrain_size, 2.2f, 0.5f, octaves, seed) * 1.5f;
            float h11 = noiseLayer((float)(x+1) / terrain_size, (float)(z+1) / terrain_size, 2.2f, 0.5f, octaves, seed) * 1.5f;
             */
            
            float h00 = sin((float)x/10.0f) * cos((float)z/10.0f) * 5;
            float h10 = sin((float)(x + 1)/10.0f) * cos((float)z/10.0f) * 5;
            float h01 = sin((float)x/10.0f) * cos((float)(z + 1)/10.0f) * 5;
            float h11 = sin((float)(x + 1)/10.0f) * cos((float)(z + 1)/10.0f) * 5;
            
            glm::vec3 p00 = glm::vec3(x     - terrain_size / 2.0f, h00, z     - terrain_size / 2.0f);
            glm::vec3 p10 = glm::vec3((x+1) - terrain_size / 2.0f, h10, z     - terrain_size / 2.0f);
            glm::vec3 p01 = glm::vec3(x     - terrain_size / 2.0f, h01, (z+1) - terrain_size / 2.0f);
            glm::vec3 p11 = glm::vec3((x+1) - terrain_size / 2.0f, h11, (z+1) - terrain_size / 2.0f);
            
            glm::vec3 averagePoint = (p00 + p10 + p01 + p11) / glm::vec3(4.0f);
            float dist = glm::distance(glm::vec3(averagePoint.x, 0, averagePoint.z), origin);
            
            if (dist < 10.0f) {
                currentHeight = averagePoint.y;
            }
            
            auto updateHeight = [&](float h) {
                float fullH = h + offset;
                if (fullH > maxHeight) maxHeight = fullH;
                if (fullH < minHeight) minHeight = fullH;
            };
            updateHeight(h00);
            updateHeight(h10);
            updateHeight(h01);
            updateHeight(h11);
            
            glm::vec3 normal1 = -CalculateNormalVector(p00, p10, p01);
            unsigned base1 = vertices.size();
            vertices.emplace_back(Vertex{p00, normal1, glm::vec2{0.0f, 0.0f}});
            vertices.emplace_back(Vertex{p10, normal1, glm::vec2{1.0f, 0.0f}});
            vertices.emplace_back(Vertex{p01, normal1, glm::vec2{0.0f, 1.0f}});
            indices.push_back(base1 + 0);
            indices.push_back(base1 + 1);
            indices.push_back(base1 + 2);
            
            glm::vec3 normal2 = -CalculateNormalVector(p10, p11, p01);
            unsigned base2 = vertices.size();
            vertices.emplace_back(Vertex{p10, normal2, glm::vec2{1.0f, 0.0f}});
            vertices.emplace_back(Vertex{p11, normal2, glm::vec2{1.0f, 1.0f}});
            vertices.emplace_back(Vertex{p01, normal2, glm::vec2{0.0f, 1.0f}});
            indices.push_back(base2 + 0);
            indices.push_back(base2 + 1);
            indices.push_back(base2 + 2);
        }
    }
    
    terrain->vertices = vertices;
    terrain->indices = indices;
    
    static_cast<Terrain*>(terrain)->colliders.clear();
    int num_chunks = (terrain_size - 1) / chunk_quads;

    for (int cx = 0; cx < num_chunks; ++cx) {
        for (int cz = 0; cz < num_chunks; ++cz) {

            ConvexCollider* col = new ConvexCollider();
            col->vertices.clear();

            int base_x = cx * chunk_quads;
            int base_z = cz * chunk_quads;

            float minY = +FLT_MAX;
            float maxY = -FLT_MAX;

            for (int lx = 0; lx <= chunk_quads; ++lx) {
                for (int lz = 0; lz <= chunk_quads; ++lz) {
                    int gx = base_x + lx;
                    int gz = base_z + lz;

                    if (gx >= terrain_size || gz >= terrain_size)
                        continue;

                    float h = sin((float)gx / 10.0f) * cos((float)gz / 10.0f) * 5;
                    glm::vec3 p(
                        gx - terrain_size / 2.0f,
                        h,
                        gz - terrain_size / 2.0f
                    );

                    minY = std::min(minY, p.y);
                    maxY = std::max(maxY, p.y);

                    col->vertices.push_back({ p, {}, {} });
                }
            }

            float thickness = 5.0f;
            float bottomY = minY - thickness;

            size_t topCount = col->vertices.size();
            for (size_t i = 0; i < topCount; ++i) {
                glm::vec3 p = col->vertices[i].vertex;
                col->vertices.push_back({ {p.x, bottomY, p.z}, {}, {} });
            }

            col->aabb_min = glm::vec3(+FLT_MAX);
            col->aabb_max = glm::vec3(-FLT_MAX);

            for (const auto& v : col->vertices) {
                col->aabb_min = glm::min(col->aabb_min, v.vertex);
                col->aabb_max = glm::max(col->aabb_max, v.vertex);
            }

            glm::vec3 center = (col->aabb_min + col->aabb_max) * 0.5f;

            for (auto& v : col->vertices)
                v.vertex -= center;

            col->position = center;
            col->scale = glm::vec3(1.0f);

            static_cast<Terrain*>(terrain)->colliders.push_back(col);
        }
    }
    
    terrain->position = glm::vec3(0.0f, 0, 0.0f);
    terrain->rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    terrain->scale = glm::vec3(1.0f);
    
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

} // namespace core

#endif /* terrain_h */
