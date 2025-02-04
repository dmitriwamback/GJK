//
//  support.h
//  GJK
//
//  Created by Dmitri Wamback on 2025-02-01.
//

#ifndef support_h
#define support_h

namespace core {

//------------------------------------------------------------------------------------------//
// Helper
//------------------------------------------------------------------------------------------//

glm::vec3 GetFurthestPoint(std::vector<glm::vec3> vertices, glm::vec3 direction) {

    glm::vec3 max = vertices[0];
    float dstMax = glm::dot(max, direction);

    for (const glm::vec3& vertex : vertices) {
        float dst = glm::dot(vertex, direction);
        if (dst > dstMax) {
            dstMax = dst;
            max = vertex;
        }
    }

    return max;
}

bool SameDirection(glm::vec3 direction, glm::vec3& AO) {
    return glm::dot(direction, AO) > 0;
}

//------------------------------------------------------------------------------------------//
// Support
//------------------------------------------------------------------------------------------//

glm::vec3 Support(const std::vector<float> colliderVertices, glm::vec3 direction) {

    std::vector<glm::vec3> vertices;
    
    for (int i = 0; i < colliderVertices.size()/3; i++) {
        const glm::vec3& vertex = glm::vec3(colliderVertices[i * 3], colliderVertices[i * 3 + 1], colliderVertices[i * 3 + 2]);
        vertices.push_back(vertex);
    }
    return GetFurthestPoint(vertices, direction);
}

}

#endif /* support_h */
