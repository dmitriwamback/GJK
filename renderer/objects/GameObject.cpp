//
// Created by Dmitri on 2026-10-23.
//

#include "GameObject.h"

#include "../../util/MathUtil.h"

std::vector<Vertex> GameObject::GetTransformedVertices() {
    glm::mat4 model = MathUtil::ComputeModelMatrix(position, rotation, scale);

    std::vector<Vertex> transformedVertices;
    for (Vertex vertex : vertices) {
        Vertex v{};

        glm::vec4 transformedPosition = model * glm::vec4(vertex.position, 1.0f);

        v.position = glm::vec3(transformedPosition.x, transformedPosition.y, transformedPosition.z);
        transformedVertices.push_back(v);
    }

    return transformedVertices;
}

std::vector<Triangle> GameObject::BuildTriangleMesh() {

    std::vector<Triangle> triangles;
    std::vector<Vertex> transformedVertices = GetTransformedVertices();

    if (!indices.empty()) {
        for (size_t i = 0; i < indices.size(); i += 3) {
            const glm::vec3& a = transformedVertices[indices[i]].position;
            const glm::vec3& b = transformedVertices[indices[i + 1]].position;
            const glm::vec3& c = transformedVertices[indices[i + 2]].position;
            triangles.emplace_back(a, b, c);
        }
    }
    else {
        for (size_t i = 0; i < transformedVertices.size(); i += 3) {
            const glm::vec3& a = transformedVertices[i].position;
            const glm::vec3& b = transformedVertices[i + 1].position;
            const glm::vec3& c = transformedVertices[i + 2].position;
            triangles.emplace_back(a, b, c);
        }
    }

    return triangles;
}

std::optional<Intersection> GameObject::Intercept(Ray ray) {
    return Raycast::PerformRaycast(ray, BuildTriangleMesh());
}

void GameObject::Render(Shader shader) {}