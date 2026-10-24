//
// Created by Dmitri on 2026-10-23.
//

#ifndef GJK_CUBE_H
#define GJK_CUBE_H
#include "GameObject.h"


class Cube: public GameObject {
public:
    Cube();
    void Render(Shader shader) override;
};


#endif //GJK_CUBE_H
