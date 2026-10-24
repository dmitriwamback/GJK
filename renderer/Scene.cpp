//
// Created by Dmitri on 2026-10-23.
//

#include "Scene.h"

#include <iostream>
#include <thread>
#include <ostream>

#include "ShaderSources.h"
#include "../collision/EPA.h"
#include "../collision/GJK.h"

Scene::Scene() {
    shader = Shader(DEFAULT_VERTEX_SHADER_SOURCE, DEFAULT_FRAGMENT_SHADER_SOURCE);
}

void Scene::Populate(OctreeNode* rootNode, GameObject *gameObject) {
    gameObjects.push_back(gameObject);
    InsertObject(rootNode, gameObject);
}

void Scene::Collide(OctreeNode* rootNode, Camera& camera) {

    for (GameObject* gameObject : GetCandidateGameObjects(rootNode, camera)) {
        Collision collision = GJK::PerformGJK(gameObject->GetTransformedVertices(), camera.GetTransformedVertices());
        if (collision.collided) {
            camera.position += collision.normal * collision.depth;
        }
    }
}

void Scene::Render(Camera camera) {
    shader.Use();
    shader.SetMatrix4("projection", camera.projection);
    shader.SetMatrix4("lookAt", camera.lookAt);

    for (GameObject* gameObject : gameObjects) {
        gameObject->Render(shader);
    }
}

void Scene::CastRay(Ray ray) {

    std::thread([](Ray ray, std::vector<GameObject*>& gameObjects) {
        for (GameObject* gameObject : gameObjects) {
            if (gameObject->Intercept(ray) != std::nullopt) {
                gameObject->color = glm::vec3(1.0f, 0.0f, 0.0f);
            }
            else {
                gameObject->color = glm::vec3(1.0f);
            }
        }
    }, ray, std::ref(gameObjects)).detach();
}


std::vector<GameObject*> Scene::GetCandidateGameObjects(OctreeNode* rootNode, Camera& camera) {
    std::vector<GameObject*> candidates;

    glm::vec3 queryMin = camera.position - glm::vec3(20.0f);
    glm::vec3 queryMax = camera.position + glm::vec3(20.0f);

    if (rootNode->children[0]) {
        candidates = ParallelQuery(rootNode, queryMin, queryMax);
    }
    else {
        QueryObjects(rootNode, queryMin, queryMax, candidates);
    }

    return candidates;
}