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
    
    float deltaScroll;
    
    float lastMouseX, lastMouseY;
    
    int mouseButton = GLFW_MOUSE_BUTTON_RIGHT;
    
    static void Initialize();
    void Update(glm::vec4 movement, float up, float down);
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
    
    std::cout << camera.pitch << " " << camera.yaw << '\n';
    std::cout << camera.position.x << " " << camera.position.y << " " << camera.position.z << '\n';
    
    camera.lastMouseX = xpos;
    camera.lastMouseY = ypos;
    camera.deltaScroll = 0;
}

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.deltaScroll = yoffset;
}

}

#endif /* camera_h */
