//
//  gjk.h
//  GJK
//
//  Created by Dmitri Wamback on 2025-01-02.
//

#ifndef gjk_h
#define gjk_h

#include <algorithm>
#include <glm/gtx/norm.hpp>

namespace core {

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

bool CollideWithCamera(Cube testCube) {
    
    glm::vec3 direction = glm::normalize(camera.position);
    std::vector<glm::vec3> simplex;
    std::vector<float> projectedVertices = testCube.GetColliderVertices();
    
    glm::vec3 supportVertex = support(projectedVertices, testCube.indices, direction) - camera.position;
    simplex.push_back(supportVertex);
    direction = -supportVertex;
    
    const int MAX_ITERATIONS = 100;
    int iteration = 0;
    
    while (iteration < MAX_ITERATIONS) {
        
        glm::vec3 newPoint = support(projectedVertices, testCube.indices, direction) - camera.position;
        
        if (glm::dot(newPoint, direction) <= 0.01f) return false;
        
        simplex.push_back(newPoint);
        glm::vec3 ao = -simplex.back();
                
        if (simplex.size() == 2) {
            
            glm::vec3 ab = simplex[0] - simplex[1];
            
            if (glm::dot(ab, ao) > 0) {
                direction = glm::cross(glm::cross(ab, ao), ab);
            } else {
                simplex.erase(simplex.begin());
                direction = ao;
            }
        }
        else if (simplex.size() == 3) {
            
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
        }
        else if (simplex.size() == 4) {
            
            glm::vec3 ab = simplex[2] - simplex[3],
                      ac = simplex[1] - simplex[3],
                      ad = simplex[0] - simplex[3];
            
            glm::vec3 abc = glm::cross(ab, ac),
                      acd = glm::cross(ac, ad),
                      adb = glm::cross(ad, ab);
            
            if (glm::dot(abc, ao) > 0) {
                simplex.erase(simplex.begin());
                direction = abc;
            } else if (glm::dot(acd, ao) > 0) {
                simplex.erase(simplex.begin() + 1);
                direction = acd;
            } else if (glm::dot(adb, ao) > 0) {
                simplex.erase(simplex.begin() + 2);
                direction = adb;
            } else {
                return true;
            }
        }
        
        iteration++;
    }
    
    return false;
}

void CameraCollisionResponse(Cube cube) {
    
    Ray ray{camera.position, camera.velocity};
    if (glm::length(camera.velocity) > 0) {
        ray.direction = glm::normalize(camera.velocity);
    }
    
    auto intersection = core::Raycast(ray, cube.GetColliderVertices(), cube.indices);
    if (intersection) {
        glm::vec3 collisionPoint = intersection->intersectionPoint;
        
        glm::vec3 centerToPoint = collisionPoint - cube.position;
        glm::vec3 normal = glm::normalize(centerToPoint);
                
        //camera.position = collisionPoint + normal;
    }
}
}

#endif /* gjk_h */
