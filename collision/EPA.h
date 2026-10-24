//
// Created by Dmitri on 2026-10-23.
//

#ifndef GJK_EPA_H
#define GJK_EPA_H
#include "Simplex.h"
#include "../renderer/objects/Vertex.h"

struct Collision {
    bool collided = false;
    glm::vec3 normal;
    float depth;
};

class EPA {
public:
    static std::pair<std::vector<glm::vec4>, size_t> GetNormal(std::vector<glm::vec3>& polytope, std::vector<size_t> indices);
    static void AddUnique(std::vector<std::pair<size_t, size_t>>& edges, const std::vector<size_t>& faces, size_t a, size_t b);
    static Collision PerformEPA(Simplex& simplex, std::vector<Vertex> A, std::vector<Vertex> B);
};


#endif //GJK_EPA_H
