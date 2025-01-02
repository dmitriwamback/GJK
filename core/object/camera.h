//
//  camera.h
//  GJK
//
//  Created by Dmitri Wamback on 2024-12-30.
//

#ifndef camera_h
#define camera_h

namespace gjk::core {

class Camera {
public:
    glm::vec3 position, lookDirection;
    glm::mat4 projection, lookAt;
    
    float pitch;
    float yaw = 3.0f * 3.14159265358f/2.0f;
    
    float lastMouseX, lastMouseY;
    
    int mouseButton = GLFW_MOUSE_BUTTON_RIGHT;
    
    static void Initialize();
    void Update(glm::vec4 movement);
};

Camera camera;

void Camera::Initialize() {
    camera = Camera();
    
    camera.position = glm::vec3(0.0f, 6.0f, 0.0f);
    camera.lookDirection = glm::vec3(0.0f, 0.0f, -1.0f);
    
    camera.projection = glm::perspective(3.14159265358f/2.0f, 3.0f/2.0f, 0.1f, 1000.0f);
}

void Camera::Update(glm::vec4 movement) {
    
    float forward = movement.x,
          backward = movement.y,
          left = movement.z,
          right = movement.w;
    
    glm::vec3 motion = lookDirection;
    
    position += motion * (forward + backward) * 10.0f;
    position -= glm::normalize(glm::cross(motion, glm::vec3(0.0f, 1.0f, 0.0f))) * (left + right) * 10.0f;
    
    lookDirection = glm::normalize(glm::vec3(cos(camera.yaw) * cos(camera.pitch),
                                             sin(camera.pitch),
                                             sin(camera.yaw) * cos(camera.pitch)));
    
    lookAt = glm::lookAt(position, position + lookDirection, glm::vec3(0.0f, 1.0f, 0.0f));
    
    int width, height;
    glfwGetWindowSize(gjk::window, &width, &height);
    float aspect = (float)width / (float)height;
    
    projection = glm::perspective(3.14159265358f/2.0f, aspect, 0.1f, 1000.0f);
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    
    if (glfwGetMouseButton(gjk::window, camera.mouseButton)) {
        
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
    
    std::cout << camera.pitch << " " << camera.yaw << '\n';
    std::cout << camera.position.x << " " << camera.position.y << " " << camera.position.z << '\n';
    
    camera.lastMouseX = xpos;
    camera.lastMouseY = ypos;
}

}

#endif /* camera_h */
