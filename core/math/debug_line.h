//
//  debug_line.h
//  GJK
//
//  Created by Dmitri Wamback on 2025-01-18.
//

#ifndef debug_line_h
#define debug_line_h

namespace core {

uint32_t debugLineVertexArrayObject;
uint32_t debugLineVertexBufferObject;

bool debugLineInitialized = false;

void RenderDebugLine(glm::vec3 a, glm::vec3 b, Shader shader) {
    
    std::vector<float> vertices = {
        a.x, a.y, a.z, 0.0f, 0.0f, 0.0f,
        b.x, b.y, b.z, 0.0f, 0.0f, 0.0f,
    };
    
    std::cout << "--------------------------DEBUG LINE VERTICES--------------------------\n";
    for (int i = 0; i < vertices.size()/6; i++) {
        std::cout << vertices[i * 6] << ", " << vertices[i * 6 + 1] << ", " << vertices[i * 6 + 2] << "\n";
    }
    
    if (!debugLineInitialized) {
        
        glGenVertexArrays(1, &debugLineVertexArrayObject);
        glBindVertexArray(debugLineVertexArrayObject);
        glGenBuffers(1, &debugLineVertexBufferObject);
        
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        
        debugLineInitialized = true;
    }
    
    glBindVertexArray(debugLineVertexArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, debugLineVertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_DYNAMIC_DRAW);
    
    shader.Use();
    shader.SetVector3("color", glm::vec3(0.0f, 1.0f, 1.0f));
    shader.SetMatrix4("projection", camera.projection);
    shader.SetMatrix4("lookAt", camera.lookAt);
    
    glm::mat4 model = glm::mat4(1.0f);
    shader.SetMatrix4("model", model);
    
    glDrawArrays(GL_POINTS, 0, 2);
    glDrawArrays(GL_LINES, 0, 2);
    
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
}

#endif /* debug_line_h */
