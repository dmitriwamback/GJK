// gjk.h
// GJK
// Created by Dmitri Wamback on 2025-01-02.

#ifndef gjk_h
#define gjk_h

#include <algorithm>
#include <glm/gtx/norm.hpp>

#include "debug_line.h"

namespace core {

//------------------------------------------------------------------------------------------//
// Helper
//------------------------------------------------------------------------------------------//

glm::vec3 GetFurthestPoint(std::vector<glm::vec3> vertices, glm::vec3 direction) {

    glm::vec3 max = vertices[0];
    float dstMax = glm::dot(max, direction);

    for (const glm::vec3& vertex : vertices) {
        float dst = glm::dot(vertex, direction);
        if (dst > dstMax) {
            dstMax = dst;
            max = vertex;
        }
    }

    return max;
}

bool SameDirection(glm::vec3 direction, glm::vec3& AO) {
    return glm::dot(direction, AO) > 0;
}

//------------------------------------------------------------------------------------------//
// Support
//------------------------------------------------------------------------------------------//

glm::vec3 Support(const std::vector<float> colliderVertices, glm::vec3 direction) {

    std::vector<glm::vec3> vertices;
    
    for (int i = 0; i < colliderVertices.size()/3; i++) {
        const glm::vec3& vertex = glm::vec3(colliderVertices[i * 3], colliderVertices[i * 3 + 1], colliderVertices[i * 3 + 2]);
        vertices.push_back(vertex);
    }
    return GetFurthestPoint(vertices, direction);
}

//------------------------------------------------------------------------------------------//
// Simplex Line
//------------------------------------------------------------------------------------------//

bool SimplexLine(Simplex& simplex, glm::vec3& direction) {
    
    glm::vec3 A = simplex[0];
    glm::vec3 B = simplex[1];
    
    glm::vec3 AB = B - A;
    glm::vec3 AO = -A;
    
    if (SameDirection(AB, AO)) {
        direction = glm::cross(glm::cross(AB, AO), AB);
    }
    else {
        simplex = { A };
        direction = AO;
    }
    
    return false;
}
//------------------------------------------------------------------------------------------//
// Simplex Triangle
//------------------------------------------------------------------------------------------//

bool SimplexTriangle(Simplex& simplex, glm::vec3& direction) {
    
    glm::vec3 A = simplex[0];
    glm::vec3 B = simplex[1];
    glm::vec3 C = simplex[2];
    
    glm::vec3 AB = B - A;
    glm::vec3 AC = C - A;
    glm::vec3 AO = -A;
    
    glm::vec3 ABC = glm::cross(AB, AC);
    
    if (SameDirection(glm::cross(ABC, AC), AO)) {
        if (SameDirection(AC, AO)) {
            simplex = {A,C};
            direction = glm::cross(glm::cross(AC, AO), AC);
        }
        else {
            return SimplexLine(simplex = {A,B}, direction);
        }
    }
    else {
        if (SameDirection(glm::cross(AB, ABC), AO)) {
            return SimplexLine(simplex = {A,B}, direction);
        }
        else {
            if (SameDirection(ABC, AO)) {
                direction = ABC;
            }
            else {
                simplex = {A, C, B};
                direction = -ABC;
            }
        }
    }

    return false;
}
//------------------------------------------------------------------------------------------//
// Simplex Tetrahedron
//------------------------------------------------------------------------------------------//

bool SimplexTetrahedron(Simplex& simplex, glm::vec3& direction) {
    
    glm::vec3 A = simplex[0];
    glm::vec3 B = simplex[1];
    glm::vec3 C = simplex[2];
    glm::vec3 D = simplex[3];
    
    glm::vec3 AB = B - A;
    glm::vec3 AC = C - A;
    glm::vec3 AD = D - A;
    glm::vec3 AO = -A;
    
    glm::vec3 ABC = glm::cross(AB, AC);
    glm::vec3 ACD = glm::cross(AC, AD);
    glm::vec3 ADB = glm::cross(AD, AB);
    
    if (SameDirection(ABC, AO)) {
        return SimplexTriangle(simplex = {A, B, C}, direction);
    }
    if (SameDirection(ACD, AO)) {
        return SimplexTriangle(simplex = {A, C, D}, direction);
    }
    if (SameDirection(ADB, AO)) {
        return SimplexTriangle(simplex = {A, D, B}, direction);
    }
    
    
    return true;
}

//------------------------------------------------------------------------------------------//
// Simplex
//------------------------------------------------------------------------------------------//

bool HandleSimplex(Simplex& simplex, glm::vec3& direction) {
    
    switch (simplex.size()) {
        case 2: return SimplexLine(simplex, direction);
        case 3: return SimplexTriangle(simplex, direction);
        case 4: return SimplexTetrahedron(simplex, direction);
    }
    
    return false;
}

//------------------------------------------------------------------------------------------//
// GJK
//------------------------------------------------------------------------------------------//

bool GJKCollision(Cube a, Cube b) {
    std::vector<float> colliderVerticesA = a.GetColliderVertices();
    std::vector<float> colliderVerticesB = b.GetColliderVertices();
    
    glm::vec3 support = Support(colliderVerticesA, glm::vec3(1.0f, 0.0f, 0.0f)) - Support(colliderVerticesB, glm::vec3(1.0f, 0.0f, 0.0f));
    
    Simplex simplex;
    simplex.pushFront(support);
    
    glm::vec3 direction = -support;
    
    for (int i = 0; i < 100; i++) {
        glm::vec3 va = Support(colliderVerticesA,  direction);
        glm::vec3 vb = Support(colliderVerticesB, -direction);
        support = va - vb;
        
        //RenderDebugLine(va, vb, shader);

        if (glm::dot(support, direction) <= 0.0f) {
            return false;
        }

        simplex.pushFront(support);

        if (HandleSimplex(simplex, direction)) {
            return true;
        }
    }

    std::cout << "Terminated: Max iterations reached. No collision detected.\n";
    return false;
}

bool GJKCollisionWithCamera(Cube a) {
    std::vector<float> colliderVerticesA = a.GetColliderVertices();
    std::vector<float> colliderVerticesB = camera.GetColliderVertices();
    
    glm::vec3 support = Support(colliderVerticesA, glm::vec3(1.0f, 0.0f, 0.0f)) - Support(colliderVerticesB, glm::vec3(1.0f, 0.0f, 0.0f));
    
    Simplex simplex;
    simplex.pushFront(support);
    
    glm::vec3 direction = -support;
    
    for (int i = 0; i < 100; i++) {
        glm::vec3 va = Support(colliderVerticesA,  direction);
        glm::vec3 vb = Support(colliderVerticesB, -direction);
        support = va - vb;

        if (glm::dot(support, direction) <= 0.0f) {
            return false;
        }

        simplex.pushFront(support);

        if (HandleSimplex(simplex, direction)) {
            return true;
        }
    }

    std::cout << "Terminated: Max iterations reached. No collision detected.\n";
    return false;
}

}

#endif /* gjk_h */
