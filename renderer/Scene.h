//
// Created by Dmitri on 2026-10-23.
//

#ifndef GJK_SCENE_H
#define GJK_SCENE_H
#include <vector>

#include "../util/OctreeNode.h"
#include "camera/Camera.h"
#include "objects/GameObject.h"


class Scene {
public:
    Scene();
    std::vector<GameObject*> gameObjects;
    Shader shader;

    void Populate(OctreeNode* rootNode, GameObject* gameObject);
    void Render(Camera camera);
    void Collide(OctreeNode* rootNode, Camera& camera);
    void CastRay(Ray ray);

private:
    std::vector<GameObject*> GetCandidateGameObjects(OctreeNode* rootNode, Camera& camera);
};


#endif //GJK_SCENE_H
