//
//  octree_node.h
//  GJK
//
//  Created by Dmitri Wamback on 2025-11-07.
//

#ifndef octree_node_h
#define octree_node_h

#include <array>
#include <memory>
#include <vector>
#include <mutex>
#include <shared_mutex>
#include <future>
#include <algorithm>
#include <glm/glm.hpp>

namespace core {

struct OctreeNode {
    glm::vec3 min;
    glm::vec3 max;

    std::vector<RObject*> objects;

    std::array<std::unique_ptr<OctreeNode>, 8> children;

    mutable std::shared_mutex nodeMutex;

    OctreeNode() = default;
    OctreeNode(const glm::vec3& a, const glm::vec3& b) : min(a), max(b) {}
    ~OctreeNode() = default;

    inline bool Intersects(const glm::vec3& amin, const glm::vec3& amax,
                           const glm::vec3& bmin, const glm::vec3& bmax) const {
        
        return (amin.x <= bmax.x && amax.x >= bmin.x) &&
               (amin.y <= bmax.y && amax.y >= bmin.y) &&
               (amin.z <= bmax.z && amax.z >= bmin.z);
    }

    inline bool ContainsFully(const glm::vec3& amin, const glm::vec3& amax,
                              const glm::vec3& bmin, const glm::vec3& bmax) const {

        return (bmin.x >= amin.x && bmax.x <= amax.x) &&
               (bmin.y >= amin.y && bmax.y <= amax.y) &&
               (bmin.z >= amin.z && bmax.z <= amax.z);
    }
};

inline void InsertObject(OctreeNode* node, RObject* obj, int depth = 0, int maxDepth = 6, int maxObjects = 8) {
    if (!node || !obj) return;

    const glm::vec3 objMin = obj->position - obj->scale * 0.5f;
    const glm::vec3 objMax = obj->position + obj->scale * 0.5f;
    {
        std::shared_lock lock(node->nodeMutex);
        if (!node->Intersects(node->min, node->max, objMin, objMax)) return;
    }

    {
        std::shared_lock lock(node->nodeMutex);
        bool hasChildren = (node->children[0] != nullptr);
        if (hasChildren) {

            int containingIndex = -1;
            for (int i = 0; i < 8; i++) {
                OctreeNode* child = node->children[i].get();
                if (!child) continue;
                if (node->ContainsFully(child->min, child->max, objMin, objMax)) {
                    containingIndex = i;
                    break;
                }
            }
            if (containingIndex >= 0) {
                InsertObject(node->children[containingIndex].get(), obj, depth + 1, maxDepth, maxObjects);
                return;
            }
            else {}
        }
    }

    {
        std::unique_lock lock(node->nodeMutex);
        node->objects.push_back(obj);

        if ((int)node->objects.size() > maxObjects && depth < maxDepth) {

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

            std::vector<RObject*> remaining;
            remaining.reserve(node->objects.size());
            for (RObject* o : node->objects) {
                const glm::vec3 oMin = o->position - o->scale * 0.5f;
                const glm::vec3 oMax = o->position + o->scale * 0.5f;

                int targetChild = -1;
                for (int i = 0; i < 8; i++) {
                    OctreeNode* child = node->children[i].get();
                    if (!child) continue;
                    if (node->ContainsFully(child->min, child->max, oMin, oMax)) {
                        targetChild = i;
                        break;
                    }
                }

                if (targetChild >= 0) {
                    remaining.push_back(reinterpret_cast<RObject*>(~0));
                }
                else {
                    // keep in this node
                    remaining.push_back(o);
                }
            }

            std::vector<RObject*> oldObjects;
            oldObjects.swap(node->objects);
            lock.unlock();

            for (RObject* o : oldObjects) {
                const glm::vec3 oMin = o->position - o->scale * 0.5f;
                const glm::vec3 oMax = o->position + o->scale * 0.5f;

                int targetChild = -1;
                
                for (int i = 0; i < 8; i++) {
                    OctreeNode* child = node->children[i].get();
                    if (!child) continue;
                    if (node->ContainsFully(child->min, child->max, oMin, oMax)) {
                        targetChild = i;
                        break;
                    }
                }

                if (targetChild >= 0) {
                    InsertObject(node->children[targetChild].get(), o, depth + 1, maxDepth, maxObjects);
                }
                else {
                    std::unique_lock lock2(node->nodeMutex);
                    node->objects.push_back(o);
                }
            }
        }
    }
}

inline void QueryObjects(OctreeNode* node, const glm::vec3& queryMin, const glm::vec3& queryMax, std::vector<RObject*>& results) {
    if (!node) return;

    std::shared_lock lock1(node->nodeMutex);
    if (!node->Intersects(node->min, node->max, queryMin, queryMax)) return;

    std::shared_lock lock2(node->nodeMutex);
    for (RObject* obj : node->objects) {
        const glm::vec3 objMin = obj->position - obj->scale * 0.5f;
        const glm::vec3 objMax = obj->position + obj->scale * 0.5f;
        if (node->Intersects(objMin, objMax, queryMin, queryMax)) {
            results.push_back(obj);
        }
    }

    std::array<OctreeNode*, 8> childRaw{};
    std::shared_lock lock(node->nodeMutex);
    for (int i = 0; i < 8; i++) {
        childRaw[i] = node->children[i].get();
    }

    for (int i = 0; i < 8; i++) {
        OctreeNode* child = childRaw[i];
        if (!child) continue;
        if (!child->Intersects(child->min, child->max, queryMin, queryMax)) continue;
        QueryObjects(child, queryMin, queryMax, results);
    }
}

inline std::vector<RObject*> ParallelQuery(OctreeNode* root, const glm::vec3& minBox, const glm::vec3& maxBox, int parallelDepth = 1, int currentDepth = 0) {
    std::vector<RObject*> results;

    if (!root) return results;
    std::shared_lock lock1(root->nodeMutex);
    bool hasChildren = (root->children[0] != nullptr);
    if (!hasChildren || currentDepth >= parallelDepth) {
        QueryObjects(root, minBox, maxBox, results);
        return results;
    }

    std::vector<std::future<std::vector<RObject*>>> futures;
    for (int i = 0; i < 8; ++i) {
        OctreeNode* child = nullptr;
        std::shared_lock lock(root->nodeMutex);
        if (root->children[i]) child = root->children[i].get();
        if (!child) continue;

        if (!child->Intersects(child->min, child->max, minBox, maxBox)) continue;

        futures.emplace_back(std::async(std::launch::async,
            [child, minBox, maxBox, parallelDepth, currentDepth]() -> std::vector<RObject*> {
                return ParallelQuery(child, minBox, maxBox, parallelDepth, currentDepth + 1);
            }));
    }

    std::vector<RObject*> localResults;
    std::shared_lock lock2(root->nodeMutex);
    for (RObject* obj : root->objects) {
        const glm::vec3 objMin = obj->position - obj->scale * 0.5f;
        const glm::vec3 objMax = obj->position + obj->scale * 0.5f;
        if (root->Intersects(objMin, objMax, minBox, maxBox)) {
            localResults.push_back(obj);
        }
    }
    results.insert(results.end(), localResults.begin(), localResults.end());

    for (auto& fut : futures) {
        auto childRes = fut.get();
        results.insert(results.end(), childRes.begin(), childRes.end());
    }

    return results;
}

}

#endif /* octree_node_h */
