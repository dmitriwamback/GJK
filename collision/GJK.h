//
// Created by Dmitri on 2026-10-23.
//

#ifndef GJK_GJK_H
#define GJK_GJK_H
#include <glm/vec3.hpp>

#include "EPA.h"
#include "Simplex.h"
#include "../renderer/camera/Camera.h"
#include "../renderer/objects/GameObject.h"

class GJK {
public:
    static bool SimplexLine(Simplex& simplex, glm::vec3& direction);
    static bool SimplexTriangle(Simplex& simplex, glm::vec3& direction);
    static bool SimplexTetrahedron(Simplex& simplex, glm::vec3& direction);
    static bool HandleSimplex(Simplex& simplex, glm::vec3& direction);

    static Collision PerformGJK(const std::vector<Vertex>& A, const std::vector<Vertex>& B);
};


#endif //GJK_GJK_H
