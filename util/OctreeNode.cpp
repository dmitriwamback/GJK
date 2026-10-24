//
// Created by Dmitri on 2026-10-23.
//

#include "OctreeNode.h"

#include <future>
#include <mutex>
#include <algorithm>


inline void GetBounds(const GameObject* obj, glm::vec3& outMin, glm::vec3& outMax) {
    outMin = obj->position - obj->scale * 0.5f;
    outMax = obj->position + obj->scale * 0.5f;
}

// Returns the index of the first child that fully contains [bmin, bmax], or -1.
// Caller must hold node->nodeMutex (shared or unique).
inline int FindContainingChild(const OctreeNode* node, const glm::vec3& bmin, const glm::vec3& bmax) {
    for (int i = 0; i < 8; i++) {
        const OctreeNode* child = node->children[i].get();
        if (!child) continue;
        if (node->ContainsFully(child->min, child->max, bmin, bmax)) return i;
    }
    return -1;
}



bool OctreeNode::Intersects(const glm::vec3& amin, const glm::vec3& amax,
                            const glm::vec3& bmin, const glm::vec3& bmax) const {
    return (amin.x <= bmax.x && amax.x >= bmin.x) &&
           (amin.y <= bmax.y && amax.y >= bmin.y) &&
           (amin.z <= bmax.z && amax.z >= bmin.z);
}

bool OctreeNode::ContainsFully(const glm::vec3& amin, const glm::vec3& amax,
                               const glm::vec3& bmin, const glm::vec3& bmax) const {
    return (bmin.x >= amin.x && bmax.x <= amax.x) &&
           (bmin.y >= amin.y && bmax.y <= amax.y) &&
           (bmin.z >= amin.z && bmax.z <= amax.z);
}

void InsertObject(OctreeNode* node, GameObject* obj, int depth, int maxDepth, int maxObjects) {
    if (!node || !obj) return;

    glm::vec3 objMin, objMax;
    GetBounds(obj, objMin, objMax);

    // node->min / node->max never change after construction, so no lock needed here
    if (!node->Intersects(node->min, node->max, objMin, objMax)) return;

    // Try to push the object down into an existing child
    OctreeNode* target = nullptr;
    {
        std::shared_lock lock(node->nodeMutex);
        int idx = FindContainingChild(node, objMin, objMax);
        if (idx >= 0) target = node->children[idx].get();
    }
    // Children are never removed, so the pointer stays valid after unlocking
    if (target) {
        InsertObject(target, obj, depth + 1, maxDepth, maxObjects);
        return;
    }

    // Store in this node, subdividing if it's over capacity
    std::vector<std::pair<int, GameObject*>> toMove;
    {
        std::unique_lock lock(node->nodeMutex);
        node->objects.push_back(obj);

        if ((int)node->objects.size() <= maxObjects || depth >= maxDepth) return;

        if (!node->children[0]) {
            const glm::vec3 center = (node->min + node->max) * 0.5f;
            for (int i = 0; i < 8; i++) {
                glm::vec3 cmin(
                    (i & 1) ? center.x : node->min.x,
                    (i & 2) ? center.y : node->min.y,
                    (i & 4) ? center.z : node->min.z
                );
                glm::vec3 cmax(
                    (i & 1) ? node->max.x : center.x,
                    (i & 2) ? node->max.y : center.y,
                    (i & 4) ? node->max.z : center.z
                );
                node->children[i] = std::make_unique<OctreeNode>(cmin, cmax);
            }
        }

        // Partition: objects that fit in a child get moved, the rest stay here
        std::vector<GameObject*> keep;
        keep.reserve(node->objects.size());
        for (GameObject* o : node->objects) {
            glm::vec3 oMin, oMax;
            GetBounds(o, oMin, oMax);
            int idx = FindContainingChild(node, oMin, oMax);
            if (idx >= 0) toMove.emplace_back(idx, o);
            else keep.push_back(o);
        }
        node->objects.swap(keep);
    }

    // Insert into children outside our lock (children have their own mutexes)
    for (auto& [idx, o] : toMove) {
        InsertObject(node->children[idx].get(), o, depth + 1, maxDepth, maxObjects);
    }
}

void QueryObjects(OctreeNode* node, const glm::vec3& queryMin, const glm::vec3& queryMax,
                  std::vector<GameObject*>& results) {
    if (!node) return;
    if (!node->Intersects(node->min, node->max, queryMin, queryMax)) return;

    std::array<OctreeNode*, 8> childRaw{};
    {
        std::shared_lock lock(node->nodeMutex);

        for (GameObject* obj : node->objects) {
            glm::vec3 objMin, objMax;
            GetBounds(obj, objMin, objMax);
            if (node->Intersects(objMin, objMax, queryMin, queryMax)) {
                results.push_back(obj);
            }
        }

        for (int i = 0; i < 8; i++) childRaw[i] = node->children[i].get();
    }

    for (OctreeNode* child : childRaw) {
        if (!child) continue;
        if (!child->Intersects(child->min, child->max, queryMin, queryMax)) continue;
        QueryObjects(child, queryMin, queryMax, results);
    }
}

std::vector<GameObject*> ParallelQuery(OctreeNode* root, const glm::vec3& minBox, const glm::vec3& maxBox,
                                       int parallelDepth, int currentDepth) {
    std::vector<GameObject*> results;
    if (!root) return results;

    std::array<OctreeNode*, 8> childRaw{};
    bool hasChildren;
    {
        std::shared_lock lock(root->nodeMutex);
        hasChildren = (root->children[0] != nullptr);
        for (int i = 0; i < 8; i++) childRaw[i] = root->children[i].get();
    }

    if (!hasChildren || currentDepth >= parallelDepth) {
        QueryObjects(root, minBox, maxBox, results);
        return results;
    }

    std::vector<std::future<std::vector<GameObject*>>> futures;
    for (OctreeNode* child : childRaw) {
        if (!child) continue;
        if (!child->Intersects(child->min, child->max, minBox, maxBox)) continue;

        futures.emplace_back(std::async(std::launch::async,
            [child, minBox, maxBox, parallelDepth, currentDepth]() {
                return ParallelQuery(child, minBox, maxBox, parallelDepth, currentDepth + 1);
            }));
    }

    // This node's own objects, while the children run in parallel
    {
        std::shared_lock lock(root->nodeMutex);
        for (GameObject* obj : root->objects) {
            glm::vec3 objMin, objMax;
            GetBounds(obj, objMin, objMax);
            if (root->Intersects(objMin, objMax, minBox, maxBox)) {
                results.push_back(obj);
            }
        }
    }

    for (auto& fut : futures) {
        auto childRes = fut.get();
        results.insert(results.end(), childRes.begin(), childRes.end());
    }

    return results;
}