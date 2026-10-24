//
// Created by Dmitri on 2026-10-23.
//

#ifndef GJK_OCTREENODE_H
#define GJK_OCTREENODE_H


#include <array>
#include <memory>
#include <vector>
#include <shared_mutex>
#include <glm/glm.hpp>

#include "../renderer/objects/GameObject.h"

struct OctreeNode {
    glm::vec3 min;
    glm::vec3 max;

    std::vector<GameObject*> objects;
    std::array<std::unique_ptr<OctreeNode>, 8> children;

    mutable std::shared_mutex nodeMutex;

    OctreeNode() = default;
    OctreeNode(const glm::vec3& a, const glm::vec3& b) : min(a), max(b) {}
    ~OctreeNode() = default;

    bool Intersects(const glm::vec3& amin, const glm::vec3& amax, const glm::vec3& bmin, const glm::vec3& bmax) const;
    bool ContainsFully(const glm::vec3& amin, const glm::vec3& amax, const glm::vec3& bmin, const glm::vec3& bmax) const;
};

void InsertObject(OctreeNode* node, GameObject* obj, int depth = 0, int maxDepth = 6, int maxObjects = 8);

void QueryObjects(OctreeNode* node, const glm::vec3& queryMin, const glm::vec3& queryMax, std::vector<GameObject*>& results);
std::vector<GameObject*> ParallelQuery(OctreeNode* root, const glm::vec3& minBox, const glm::vec3& maxBox, int parallelDepth = 1, int currentDepth = 0);



#endif //GJK_OCTREENODE_H
