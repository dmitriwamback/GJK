//
//  camera.h
//  GJK
//
//  Created by Dmitri Wamback on 2024-12-30.
//

#ifndef camera_h
#define camera_h

namespace core {

class Camera {
public:
    glm::vec3 position, lookDirection, velocity;
    glm::mat4 projection, lookAt;
    
    glm::vec3 mouseRayDirection;
    
    float pitch;
    float yaw = 3.0f * 3.14159265358f/2.0f;
    float radius = 0.5f;
    
    float deltaScroll, lastYScroll;
    
    float lastMouseX, lastMouseY;
    
    int mouseButton = GLFW_MOUSE_BUTTON_RIGHT;
    
    std::vector<uint32_t> indices = {
        0, 2, 1, 0, 3, 2,  // Reversed (CW)

        // Back face
        5, 7, 4, 5, 6, 7,  // Reversed (CW)

        // Left face
        4, 3, 0, 4, 7, 3,  // Reversed (CW)

        // Right face
        1, 6, 5, 1, 2, 6,  // Reversed (CW)

        // Top face
        4, 1, 5, 4, 0, 1,  // Reversed (CW)

        // Bottom face
        3, 6, 2, 3, 7, 6   // Reversed (CW)
    };
    
    std::vector<float> vertices = {
        -0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
    };
    
    static void Initialize();
    void Update(glm::vec4 movement, float up, float down);
    std::vector<float> GetColliderVertices();
    glm::mat4 CreateModelMatrix();
};

Camera camera;

void Camera::Initialize() {
    camera = Camera();
    
    camera.position = glm::vec3(0.0f, 6.0f, 0.0f);
    camera.lookDirection = glm::vec3(0.0f, 0.0f, -1.0f);
    camera.velocity = glm::vec3(0.0f);
    
    camera.projection = glm::perspective(3.14159265358f/2.0f, 3.0f/2.0f, 0.1f, 1000.0f);
}

void Camera::Update(glm::vec4 movement, float up, float down) {
    
    float forward = movement.x,
          backward = movement.y,
          left = movement.z,
          right = movement.w;
    
    glm::vec3 motion = lookDirection;
    
    velocity = (motion * (forward + backward) * 2.0f) - (glm::normalize(glm::cross(motion, glm::vec3(0.0f, 1.0f, 0.0f))) * (left + right) * 2.0f) + (glm::vec3(0.0f, 1.0f, 0.0f) * (up + down) * 2.0f);
    
    position += velocity;
    
    lookDirection = glm::normalize(glm::vec3(cos(camera.yaw) * cos(camera.pitch),
                                             sin(camera.pitch),
                                             sin(camera.yaw) * cos(camera.pitch)));
    
    lookAt = glm::lookAt(position, position + lookDirection, glm::vec3(0.0f, 1.0f, 0.0f));
    
    int width, height;
    glfwGetWindowSize(core::window, &width, &height);
    float aspect = (float)width / (float)height;
    
    projection = glm::perspective(3.14159265358f/2.0f, aspect, 0.1f, 1000.0f);
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    
    if (glfwGetMouseButton(window, camera.mouseButton)) {
        
        float deltaX = xpos - camera.lastMouseX;
        float deltaY = ypos - camera.lastMouseY;
        
        camera.pitch -= deltaY * 0.005f;
        camera.yaw += deltaX * 0.005f;
        
        if (camera.pitch >  1.55f) camera.pitch =  1.55f;
        if (camera.pitch < -1.55f) camera.pitch = -1.55f;
        
        camera.lookDirection = glm::normalize(glm::vec3(
                                            cos(camera.yaw) * cos(camera.pitch),
                                            sin(camera.pitch),
                                            sin(camera.yaw) * cos(camera.pitch)
                                            ));
    }
    
    int width, height;
    glfwGetWindowSize(core::window, &width, &height);
    
    float x = 2.0f * camera.lastMouseX / (float)width - 1.0f;
    float y = 1.0f - (2.0f * camera.lastMouseY) / (float)height;
    
    glm::vec3 nearPlane = glm::vec3(x, y, 0.0f);
    glm::vec3 farPlane = glm::vec3(x, y, 1.0f);
    
    glm::mat4 inverseProjection = glm::inverse(camera.projection);
    
    glm::vec4 nearWorld = inverseProjection * glm::vec4(nearPlane, 1.0f);
    glm::vec4 farWorld = inverseProjection * glm::vec4(farPlane, 1.0f);
    nearWorld /= nearWorld.w;
    farWorld /= farWorld.w;
    
    glm::vec3 rayDirection = glm::normalize(glm::vec3(farWorld) - glm::vec3(nearWorld));
    camera.mouseRayDirection = glm::normalize(glm::vec3(glm::inverse(camera.lookAt) * glm::vec4(rayDirection, 0.0f)));
    
    camera.lastMouseX = xpos;
    camera.lastMouseY = ypos;
    camera.deltaScroll = 0;
}

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.deltaScroll = camera.lastYScroll - yoffset;
    camera.lastYScroll = yoffset;
}

std::vector<float> Camera::GetColliderVertices() {
    
    glm::mat4 model = CreateModelMatrix();
    
    std::vector<float> projectedVertices = std::vector<float>();
    
    for (int i = 0; i < vertices.size()/3; i++) {
        glm::vec3 vertex = glm::vec3(vertices[i * 3], vertices[i * 3 + 1], vertices[i * 3 + 2]);
        glm::vec3 projected = glm::vec3(model * glm::vec4(vertex, 1.0));
        projectedVertices.push_back(projected.x);
        projectedVertices.push_back(projected.y);
        projectedVertices.push_back(projected.z);
    }
    return projectedVertices;
    
}

glm::mat4 Camera::CreateModelMatrix() {
    
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 translationMatrix = glm::mat4(1.0f);
    translationMatrix = glm::translate(translationMatrix, position);
        
    model = translationMatrix;
    
    return model;
}

}

#endif /* camera_h */
