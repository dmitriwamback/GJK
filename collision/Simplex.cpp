//
// Created by Dmitri on 2026-10-23.
//

#include "Simplex.h"

Simplex& Simplex::operator=(std::initializer_list<glm::vec3> list) {

    for (auto v = list.begin(); v != list.end(); ++v) {
        points[std::distance(list.begin(), v)] = *v;
    }
    simplexSize = list.size();
    return *this;
}

glm::vec3& Simplex::operator[](unsigned i) {
    return points[i];
}

void Simplex::PushFront(glm::vec3 point) {
    points = {point, points[0], points[1], points[2]};
    simplexSize = std::min(simplexSize + 1, 4);
}

unsigned Simplex::Size() {
    return simplexSize;
}

std::array<glm::vec3, 4>::iterator Simplex::Begin() {
    return points.begin();
}

std::array<glm::vec3, 4>::iterator Simplex::End() {
    return points.end() - (4 - simplexSize);
}
