//
// Created by Dmitri on 2026-10-23.
//

#include "GJK.h"

#include <iostream>
#include <ostream>

#include "Support.h"


bool GJK::SimplexLine(Simplex &simplex, glm::vec3& direction) {
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

bool GJK::SimplexTriangle(Simplex &simplex, glm::vec3& direction) {
    glm::vec3 A = simplex[0];
    glm::vec3 B = simplex[1];
    glm::vec3 C = simplex[2];

    glm::vec3 AB = B - A;
    glm::vec3 AC = C - A;
    glm::vec3 AO = -A;

    glm::vec3 ABC = glm::cross(AB, AC);

    if (SameDirection(glm::cross(ABC, AC), AO)) {
        if (SameDirection(AC, AO)) {
            simplex = { A, C };
            direction = glm::cross(glm::cross(AC, AO), AC);
        }
        else {
            return SimplexLine(simplex = { A, B }, direction);
        }
    }
    else {
        if (SameDirection(glm::cross(AB, ABC), AO)) {
            return SimplexLine(simplex = { A, B }, direction);
        }
        else {
            if (SameDirection(ABC, AO)) {
                direction = ABC;
            }
            else {
                simplex = { A, C, B };
                direction = -ABC;
            }
        }
    }

    return false;
}

bool GJK::SimplexTetrahedron(Simplex &simplex, glm::vec3& direction) {

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
        return SimplexTriangle(simplex = { A, B, C }, direction);
    }
    if (SameDirection(ACD, AO)) {
        return SimplexTriangle(simplex = { A, C, D }, direction);
    }
    if (SameDirection(ADB, AO)) {
        return SimplexTriangle(simplex = { A, D, B }, direction);
    }

    return true;
}

bool GJK::HandleSimplex(Simplex &simplex, glm::vec3& direction) {
    switch (simplex.Size()) {
        case 2: return SimplexLine(simplex, direction);
        case 3: return SimplexTriangle(simplex, direction);
        case 4: return SimplexTetrahedron(simplex, direction);
    }

    return false;
}

Collision GJK::PerformGJK(const std::vector<Vertex>& A, const std::vector<Vertex>& B) {

    Collision collision{};
    collision.collided = false;

    glm::vec3 support = Support(A, glm::vec3(1.0f, 0.0f, 0.0f)) - Support(B, -glm::vec3(1.0f, 0.0f, 0.0f));

    Simplex simplex;
    simplex.PushFront(support);

    glm::vec3 direction = -support;

    for (int i = 0; i < 100; i++) {
        glm::vec3 va = Support(A, direction);
        glm::vec3 vb = Support(B, -direction);
        support = va - vb;

        if (glm::dot(support, direction) <= 0.0f) {
            return collision;
        }

        simplex.PushFront(support);

        if (HandleSimplex(simplex, direction)) {
            collision = EPA::PerformEPA(simplex, A, B);
            return collision;
        }
    }

    return collision;
}

