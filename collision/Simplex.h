//
// Created by Dmitri on 2026-10-23.
//

#ifndef GJK_SIMPLEX_H
#define GJK_SIMPLEX_H
#include <array>
#include <glm/vec3.hpp>

class Simplex {
public:
    Simplex() : simplexSize(0){}

    Simplex& operator=(std::initializer_list<glm::vec3> list);
    glm::vec3& operator[](unsigned int i);

    void PushFront(glm::vec3 point);
    unsigned Size();
    std::array<glm::vec3, 4>::iterator Begin();
    std::array<glm::vec3, 4>::iterator End();

private:
    std::array<glm::vec3, 4> points;
    int simplexSize;
};


#endif //GJK_SIMPLEX_H
