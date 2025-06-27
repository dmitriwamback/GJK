//
//  epa.h
//  GJK
//
//  Created by Dmitri Wamback on 2025-01-19.
//

#ifndef epa_h
#define epa_h

namespace core {

struct collision {
    glm::vec3 A, B, normal;
    float depth;
    bool collided;
};

std::pair<std::vector<glm::vec4>, size_t> GetNormal(std::vector<glm::vec3>& polytope, std::vector<size_t> indices) {
    
    std::vector<glm::vec4> normals;
    size_t min = 0;
    float mindst = FLT_MAX;
    
    for (int i = 0; i < indices.size()/3; i++) {
        
        glm::vec3 A = polytope[indices[i*3]];
        glm::vec3 B = polytope[indices[i*3 + 1]];
        glm::vec3 C = polytope[indices[i*3 + 2]];
        
        glm::vec3 normal = glm::cross(B - A, C - A);
        if (glm::length(normal) < 1e-6f) continue;
        normal = glm::normalize(normal);
        float dst = dot(normal, A);
        if (dst < 0) {
            normal = -normal;
            dst = -dst;
        }
        normals.emplace_back(normal, dst);
        if (dst < mindst) {
            min = i;
            mindst = dst;
        }
    }
    return {normals, min};
}

void AddUnique(std::vector<std::pair<size_t, size_t>>& edges, const std::vector<size_t>& faces, size_t a, size_t b) {
    auto reverse = std::find_if(edges.begin(), edges.end(),
        [&](const std::pair<size_t, size_t>& edge) {
            return (edge.first == faces[b] && edge.second == faces[a]);
        });

    if (reverse != edges.end()) {
        edges.erase(reverse);
    } else {
        edges.emplace_back(faces[a], faces[b]);
    }
}

collision EPA(Simplex& simplex, std::vector<Vertex> colliderA, std::vector<Vertex> colliderB) {
    
    collision collisionDetection{};
    collisionDetection.normal = glm::vec3(0.0f);
    collisionDetection.depth = 0.0f;
    collisionDetection.collided = false;
    
    if (simplex.size() < 4) return collisionDetection;
    
    
    std::vector<glm::vec3> polytope(simplex.begin(), simplex.end());
    std::vector<size_t> indices = {
        0, 1, 2,    0, 3, 1,
        0, 2, 3,    1, 3, 2
    };
    
    auto [normals, minTriangle] = GetNormal(polytope, indices);
    glm::vec3 min;
    float mindst = FLT_MAX;
    
    for(int k = 0; k < 100; k++) {
        min = glm::vec3(normals[minTriangle]);
        mindst = normals[minTriangle].w;
        
        glm::vec3 support = Support(colliderA, min) - Support(colliderB, -min);
        if (glm::length(support) < 1e-6f) break;
        float sdst = glm::dot(min, support);
        
        if (abs(sdst - mindst) > 0.001f && sdst < 1e6f) {
            
            mindst = FLT_MAX;
            std::vector<std::pair<size_t, size_t>> unique;
            
            for (size_t i = 0; i < normals.size(); i++) {
                if (glm::dot(glm::vec3(normals[i]), support) > normals[i].w) {
                    size_t index = i*3;

                    AddUnique(unique, indices, index,     index + 1);
                    AddUnique(unique, indices, index + 1, index + 2);
                    AddUnique(unique, indices, index + 2, index);

                    indices[index+2] = indices.back();
                    indices.pop_back();
                    indices[index+1] = indices.back();
                    indices.pop_back();
                    indices[index] = indices.back();
                    indices.pop_back();
                    
                    normals[i] = normals.back();
                    normals.pop_back();

                    i--;
                }
            }
            std::vector<size_t> faces = std::vector<size_t>();
            for (auto [i, j] : unique) {
                faces.push_back(i);
                faces.push_back(j);
                faces.push_back(polytope.size());
            }
            
            
            polytope.push_back(support);
            
            auto [newNormals, newMinFace] = GetNormal(polytope, faces);
            
            float oldMinDistance = FLT_MAX;
            for (size_t i = 0; i < normals.size(); i++) {
                if (normals[i].w < oldMinDistance) {
                    oldMinDistance = normals[i].w;
                    minTriangle = i;
                }
            }
 
            if (newNormals[newMinFace].w < oldMinDistance) {
                minTriangle = newMinFace + normals.size();
            }
 
            indices.insert(indices.end(), faces.begin(), faces.end());
            normals.insert(normals.end(), newNormals.begin(), newNormals.end());
        }
        
    }
     
    collisionDetection.normal = min;
    collisionDetection.depth = std::min(mindst + 0.001f, 1e2f);
    collisionDetection.collided = true;
    
    return collisionDetection;
}

}

#endif /* epa_h */
