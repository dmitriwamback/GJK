//
// Created by Dmitri on 2026-10-23.
//

#include "EPA.h"

#include <cfloat>

#include "Support.h"


std::pair<std::vector<glm::vec4>, size_t> EPA::GetNormal(std::vector<glm::vec3> &polytope, std::vector<size_t> indices) {

    std::vector<glm::vec4> normals;
    size_t min = 0;
    float mindst = FLT_MAX;

    for (int i = 0; i < indices.size()/3; i++) {
        glm::vec3 A = polytope[indices[i * 3]];
        glm::vec3 B = polytope[indices[i * 3 + 1]];
        glm::vec3 C = polytope[indices[i * 3 + 2]];

        glm::vec3 normal = glm::cross(B - A, C - A);
        float magnitude = glm::length(normal);

        if (magnitude < 0.000001f) continue;

        normal = glm::normalize(normal);
        float dst = dot(normal, A);

        if (dst < 0) {
            normal = -normal;
            dst = -dst;
        }
        normals.emplace_back(normal, dst);

        if (dst < mindst) {
            min = i;
            mindst = dst;
        }
    }

    return {normals, min};
}

void EPA::AddUnique(std::vector<std::pair<size_t, size_t> > &edges, const std::vector<size_t> &faces, size_t a, size_t b) {
    auto reverse = std::find_if(edges.begin(), edges.end(),
        [&](const std::pair<size_t, size_t> &edge) {
            return (edge.first == faces[b] && edge.second == faces[a]);
        }
    );

    if (reverse != edges.end()) {
        edges.erase(reverse);
    }
    else {
        edges.emplace_back(faces[a], faces[b]);
    }
}

Collision EPA::PerformEPA(Simplex &simplex, std::vector<Vertex> A, std::vector<Vertex> B) {
    Collision collision;
    collision.collided = false;
    collision.normal = glm::vec3(0.0f);
    collision.depth = 0.0f;

    if (simplex.Size() < 4) return collision;

    std::vector<glm::vec3> polytope(simplex.Begin(), simplex.End());
    std::vector<size_t> indices = {
        0, 1, 2,    0, 3, 1,
        0, 2, 3,    1, 3, 2
    };

    auto [normals, minTriangle] = GetNormal(polytope, indices);
    glm::vec3 min;
    float mindst = FLT_MAX;

    for (int k = 0; k < 100; k++) {
        min = glm::vec3(normals[minTriangle]);
        mindst = normals[minTriangle].w;

        glm::vec3 support = Support(A, min) - Support(B, -min);
        if (glm::length(support) < 1e-6f) break;

        float sdst = glm::dot(min, support);

        if (fabs(sdst - mindst) > 0.001f && sdst < 1e6f) {

            mindst = FLT_MAX;
            std::vector<std::pair<size_t, size_t>> unique;

            for (size_t i = 0; i < normals.size(); i++) {
                if (glm::dot(glm::vec3(normals[i]), support) > normals[i].w) {
                    size_t index = i * 3;

                    AddUnique(unique, indices, index,     index + 1);
                    AddUnique(unique, indices, index + 1, index + 2);
                    AddUnique(unique, indices, index + 2, index);

                    indices[index + 2] = indices.back();
                    indices.pop_back();
                    indices[index + 1] = indices.back();
                    indices.pop_back();
                    indices[index] = indices.back();
                    indices.pop_back();

                    normals[i] = normals.back();
                    normals.pop_back();

                    i--;
                }
            }

            std::vector<size_t> faces = std::vector<size_t>();

            for (auto [i, j] : unique) {
                faces.push_back(i);
                faces.push_back(j);
                faces.push_back(polytope.size());
            }

            polytope.push_back(support);

            auto [newNormal, newMinFace] = GetNormal(polytope, faces);

            float oldmindst = FLT_MAX;
            for (size_t i = 0; i < normals.size(); i++) {
                if (normals[i].w < oldmindst) {
                    oldmindst = normals[i].w;
                    minTriangle = i;
                }
            }

            if (!newNormal.empty() && newNormal[newMinFace].w < oldmindst) {
                minTriangle = newMinFace + normals.size();
            }

            indices.insert(indices.end(), faces.begin(), faces.end());
            normals.insert(normals.end(), newNormal.begin(), newNormal.end());
        }
    }

    collision.normal = min;
    collision.depth = std::min(mindst + 0.001f, 100.0f);
    collision.collided = true;

    return collision;
}
