//
//  calculate_normal.h
//  GJK
//
//  Created by Dmitri Wamback on 2025-03-12.
//

#ifndef calculate_normal_h
#define calculate_normal_h

namespace core {

glm::vec3 CalculateNormalVector(glm::vec3 P1, glm::vec3 P2, glm::vec3 P3) {
    glm::vec3 A = P2 - P1;
    glm::vec3 B = P3 - P1;
    
    return glm::normalize(glm::cross(A, B));
}

}

#endif /* calculate_normal_h */
