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

collision GJKCollision(RObject* a, RObject* b) {
    
    std::vector<float> colliderVerticesA = a->GetColliderVertices();
    std::vector<float> colliderVerticesB = b->GetColliderVertices();
    
    collision collisionInformation{};
    collisionInformation.collided = false;
    
    glm::vec3 support = Support(colliderVerticesA, glm::vec3(1.0f, 0.0f, 0.0f)) - Support(colliderVerticesB, -glm::vec3(1.0f, 0.0f, 0.0f));
    
    Simplex simplex;
    simplex.pushFront(support);
    
    glm::vec3 direction = -support;
    
    for (int i = 0; i < 100; i++) {
        glm::vec3 va = Support(colliderVerticesA,  direction);
        glm::vec3 vb = Support(colliderVerticesB, -direction);
        support = va - vb;
        
        //RenderDebugLine(va, vb, shader);

        if (glm::dot(support, direction) <= 0.0f) {
            return collisionInformation;
        }

        simplex.pushFront(support);

        if (HandleSimplex(simplex, direction)) {
            collisionInformation = EPA(simplex, a->GetColliderVertices(), b->GetColliderVertices());
            return collisionInformation;
        }
    }

    std::cout << "Terminated: Max iterations reached. No collision detected.\n";
    return collisionInformation;
}

collision GJKCollisionWithCamera(RObject* a) {
    
    collision collisionInformation{};
    collisionInformation.collided = false;
    
    std::vector<float> colliderVerticesA = a->GetColliderVertices();
    std::vector<float> colliderVerticesB = camera.GetColliderVertices();
    
    glm::vec3 support = Support(colliderVerticesA, glm::vec3(1.0f, 0.0f, 0.0f)) - Support(colliderVerticesB, -glm::vec3(1.0f, 0.0f, 0.0f));
    
    Simplex simplex;
    simplex.pushFront(support);
    
    glm::vec3 direction = -support;
    
    for (int i = 0; i < 100; i++) {
        glm::vec3 va = Support(colliderVerticesA,  direction);
        glm::vec3 vb = Support(colliderVerticesB, -direction);
        support = va - vb;

        if (glm::dot(support, direction) <= 0.0f) {
            return collisionInformation;
        }

        simplex.pushFront(support);

        if (HandleSimplex(simplex, direction)) {
            collisionInformation = EPA(simplex, a->GetColliderVertices(), camera.GetColliderVertices());
            return collisionInformation;
        }
    }

    std::cout << "Terminated: Max iterations reached. No collision detected.\n";
    return collisionInformation;
}

}

#endif /* gjk_h */
