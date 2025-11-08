//
//  octree_node.h
//  GJK
//
//  Created by Dmitri Wamback on 2025-11-07.
//

#ifndef octree_node_h
#define octree_node_h

#include <thread>
#include <future>
#include <mutex>

namespace core {

typedef struct OctreeNode {
    glm::vec3 min, max;
    std::vector<RObject*> objects;
    OctreeNode* children[8] = {nullptr};
};

bool BoxIntersect(const glm::vec3& min1, const glm::vec3& max1, const glm::vec3& min2, const glm::vec3& max2) {
    return (min1.x <= max2.x && max1.x >= min2.x) &&
           (min1.y <= max2.y && max1.y >= min2.y) &&
           (min1.z <= max2.z && max1.z >= min2.z);
}

void InsertObject(OctreeNode* node, RObject* obj, int depth = 0, int maxDepth = 5, int maxObjects = 8) {
    glm::vec3 objMin = obj->position - obj->scale * 0.5f;
    glm::vec3 objMax = obj->position + obj->scale * 0.5f;
    
    if (!BoxIntersect(node->min, node->max, objMin, objMax)) {
        return;
    }

    if (node->children[0] != nullptr) {
        for (int i = 0; i < 8; i++) {
            if (BoxIntersect(node->children[i]->min, node->children[i]->max, objMin, objMax)) {
                InsertObject(node->children[i], obj, depth + 1, maxDepth, maxObjects);
            }
        }
        return;
    }

    node->objects.push_back(obj);

    if (node->objects.size() > maxObjects && depth < maxDepth) {

        glm::vec3 center = (node->min + node->max) * 0.5f;
        for (int i = 0; i < 8; i++) {
            node->children[i] = new OctreeNode();

            node->children[i]->min = glm::vec3(
                (i & 1) ? center.x : node->min.x,
                (i & 2) ? center.y : node->min.y,
                (i & 4) ? center.z : node->min.z
            );
            node->children[i]->max = glm::vec3(
                (i & 1) ? node->max.x : center.x,
                (i & 2) ? node->max.y : center.y,
                (i & 4) ? node->max.z : center.z
            );
        }

        std::vector<RObject*> oldObjects = node->objects;
        node->objects.clear();
        for (RObject* o : oldObjects) {
            InsertObject(node, o, depth, maxDepth, maxObjects);
        }
    }
}

void QueryObjects(OctreeNode* node, const glm::vec3& queryMin, const glm::vec3& queryMax, std::vector<RObject*>& results) {
    if (!BoxIntersect(node->min, node->max, queryMin, queryMax)) return;

    for (RObject* obj : node->objects) {
        glm::vec3 objMin = obj->position - obj->scale * 0.5f;
        glm::vec3 objMax = obj->position + obj->scale * 0.5f;
        if (BoxIntersect(objMin, objMax, queryMin, queryMax)) {
            results.push_back(obj);
        }
    }

    for (int i = 0; i < 8; i++) {
        if (node->children[i] != nullptr) {
            QueryObjects(node->children[i], queryMin, queryMax, results);
        }
    }
}

std::vector<RObject*> ParallelQuery(core::OctreeNode* root, const glm::vec3& minBox, const glm::vec3& maxBox) {
    std::vector<std::future<std::vector<RObject*>>> futures;
    
    for (int i = 0; i < 8; i++) {
        core::OctreeNode* child = root->children[i];
        if (!child) continue;

        auto worker = [child, minBox, maxBox]() {
            std::vector<RObject*> results;
            core::QueryObjects(child, minBox, maxBox, results);
            return results;
        };

        futures.push_back(std::async(std::launch::async, worker));
    }

    std::vector<RObject*> mergedResults;
    for (auto& fut : futures) {
        auto results = fut.get();
        mergedResults.insert(mergedResults.end(), results.begin(), results.end());
    }

    return mergedResults;
}


}

#endif /* octree_node_h */
