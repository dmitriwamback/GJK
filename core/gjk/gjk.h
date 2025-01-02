//
//  gjk.h
//  GJK
//
//  Created by Dmitri Wamback on 2025-01-02.
//

#ifndef gjk_h
#define gjk_h

#include <algorithm>

namespace gjk::core::math {

glm::vec3 support(const std::vector<float>& vertices, const std::vector<uint32_t>& indices, glm::vec3 direction) {
    
    float maxDot = -FLT_MAX;
    glm::vec3 bestVertex;
    
    for (uint32_t index : indices) {
        const glm::vec3& vertex = glm::vec3(vertices[index * 3], vertices[index * 3 + 1], vertices[index * 3 + 2]);
        float dot = glm::dot(vertex, direction);
        
        if (dot > maxDot) {
            maxDot = dot;
            bestVertex = vertex;
        }
    }
    return bestVertex;
}

bool CollideWithCamera(gjk::core::Cube testCube) {
    
    glm::vec3 direction = glm::normalize(gjk::core::camera.position);
    std::vector<glm::vec3> simplex;
    std::vector<float> projectedVertices = testCube.GetColliderVertices();
    
    glm::vec3 supportVertex = support(projectedVertices, testCube.indices, direction) - gjk::core::camera.position;
    simplex.push_back(supportVertex);
    direction = -supportVertex;
    
    const int MAX_ITERATIONS = 50;
    int iteration = 0;
    
    while (iteration < MAX_ITERATIONS) {
        
        glm::vec3 newPoint = support(projectedVertices, testCube.indices, direction) - gjk::core::camera.position;
        
        if (glm::dot(newPoint, direction) <= 0) return false;
        
        simplex.push_back(newPoint);
        
        glm::vec3 ao = -simplex.back();
        
        if (simplex.size() != 3) {
            iteration++;
            continue;
        }
        
        glm::vec3 ab = simplex[1] - simplex[2],
                  ac = simplex[0] - simplex[2];
        glm::vec3 abc = glm::cross(ab, ac);
        
        if (glm::dot(glm::cross(abc, ac), ao) > 0) {
            simplex.erase(simplex.begin());
            direction = glm::cross(ac, ao);
        }
        else if (glm::dot(glm::cross(ab, abc), ao) > 0) {
            simplex.erase(simplex.begin() + 1);
            direction = glm::cross(ab, ao);
        }
        else {
            return true;
        }
        direction = ao;
        
        iteration++;
    }
    
    return false;
}

}

#endif /* gjk_h */
