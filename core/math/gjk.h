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


//------------------------------------------------------------------------------------------//
// Support function for Polygon/Camera collisions
//------------------------------------------------------------------------------------------//

glm::vec3 Support(const std::vector<float>& vertices, const std::vector<uint32_t>& indices, glm::vec3 direction, glm::mat4 model) {
    
    float maxDot = -FLT_MAX;
    glm::vec3 bestVertex;
    
    glm::vec3 localDirection = glm::vec3(glm::inverse(model) * glm::vec4(direction, 0.0f));
    
    for (uint32_t index : indices) {
        const glm::vec3& vertex = glm::vec3(vertices[index * 3], vertices[index * 3 + 1], vertices[index * 3 + 2]);
        float dot = glm::dot(vertex, localDirection);
        
        if (dot > maxDot) {
            maxDot = dot;
            bestVertex = vertex;
        }
    }
    return bestVertex;
}

//------------------------------------------------------------------------------------------//
// Support function for Polygon/Polygon collisions
//------------------------------------------------------------------------------------------//

glm::vec3 Support(const std::vector<float>& verticesA, const std::vector<uint32_t>& indicesA, const std::vector<float>& verticesB, const std::vector<uint32_t>& indicesB, glm::mat4 modelA, glm::mat4 modelB, glm::vec3 direction) {
    
    auto furthest = [](const std::vector<float>& vertices, const std::vector<uint32_t>& indices, const glm::vec3& direction) -> glm::vec3 {
        
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
    };
    
    glm::vec3 pointA = furthest(verticesA, indicesA, glm::vec3(glm::inverse(modelA) * glm::vec4(direction, 0.0f)));
    glm::vec3 pointB = furthest(verticesB, indicesB, glm::vec3(glm::inverse(modelB) * glm::vec4(-direction, 0.0f)));
    
    return pointA - pointB;
}

//------------------------------------------------------------------------------------------//
// Handling simplex
//------------------------------------------------------------------------------------------//

bool SimplexHandler(std::vector<glm::vec3>& simplex, glm::vec3& direction) {
    
    glm::vec3 ao = -simplex.back();
    
    if (simplex.size() == 2) {
        
        glm::vec3 ab = simplex[0] - simplex[1];
        
        if (glm::dot(ab, ao) > 0) {
            direction = glm::cross(glm::cross(ab, ao), ab);
        } 
        else {
            simplex.erase(simplex.begin());
            direction = ao;
        }
    }
    else if (simplex.size() == 3) {
        
        glm::vec3 ab = simplex[1] - simplex[2],
                  ac = simplex[0] - simplex[2];
        glm::vec3 abc = glm::cross(ab, ac);
        
        if (glm::dot(glm::cross(abc, ac), ao) > 0) {
            simplex.erase(simplex.begin() + 1);
            direction = glm::cross(ac, ao);
        }
        else if (glm::dot(glm::cross(ab, abc), ao) > 0) {
            simplex.erase(simplex.begin() + 2);
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
    
    return false;
}

//------------------------------------------------------------------------------------------//
// Colliding between polygons
//------------------------------------------------------------------------------------------//

bool GJKCollision(Cube cube1, Cube cube2) {
    
    glm::vec3 direction = glm::normalize(cube1.position - cube2.position);
    
    std::vector<float> projectedVerticesCube1 = cube1.GetColliderVertices();
    std::vector<float> projectedVerticesCube2 = cube2.GetColliderVertices();
    
    std::vector<glm::vec3> simplex;
    
    glm::vec3 supportVertex = Support(projectedVerticesCube1, cube1.indices, projectedVerticesCube2, cube2.indices, cube1.CreateModelMatrix(), cube2.CreateModelMatrix(), direction);
    direction = -supportVertex;
    
    for (int iteration = 0; iteration < 100; iteration++) {
        
        glm::vec3 newPoint = Support(projectedVerticesCube1, cube1.indices, projectedVerticesCube2, cube2.indices, cube1.CreateModelMatrix(), cube2.CreateModelMatrix(), direction);
        if (glm::dot(newPoint, direction) <= 0.001f) {
            return false;
        }

        simplex.push_back(newPoint);

        if (SimplexHandler(simplex, direction)) {
            return true;
        }
    }
    
    return false;
}

//------------------------------------------------------------------------------------------//
// Colliding with the camera
//------------------------------------------------------------------------------------------//

bool CollideWithCamera(Cube testCube) {
    
    glm::vec3 direction = glm::normalize(camera.position);
    std::vector<glm::vec3> simplex;
    std::vector<float> projectedVertices = testCube.GetColliderVertices();
    
    glm::vec3 supportVertex = Support(projectedVertices, testCube.indices, direction, testCube.CreateModelMatrix()) - camera.position;
    simplex.push_back(supportVertex);
    direction = -supportVertex;
    
    const int MAX_ITERATIONS = 100;
    
    for (int iteration = 0; iteration < MAX_ITERATIONS; iteration++) {
        
        glm::vec3 newPoint = Support(projectedVertices, testCube.indices, direction, testCube.CreateModelMatrix()) - camera.position;
        if (glm::dot(newPoint, direction) <= 0.0000000001f) return false;
        
        simplex.push_back(newPoint);
        if (SimplexHandler(simplex, direction)) return true;
    }
    
    return false;
}


//------------------------------------------------------------------------------------------//
// Camera response to colliding
//------------------------------------------------------------------------------------------//

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
