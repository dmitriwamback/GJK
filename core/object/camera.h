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
    glm::vec3 position, lookDirection;
    glm::mat4 projection, lookAt;
    
    glm::vec3 mouseRayDirection;
    
    float pitch;
    float yaw = 3.0f * 3.14159265358f/2.0f;
    float radius = 0.5f;
    float speed = 30.0f;
    
    float deltaScroll, lastYScroll;
    
    float lastMouseX, lastMouseY;
    
    int mouseButton = GLFW_MOUSE_BUTTON_RIGHT;
    
    std::vector<Vertex> vertices = {
        Vertex({-0.5f,  0.5f,  0.5f}, glm::vec3(0.0f), glm::vec2(0.0f)),
        Vertex({ 0.5f,  0.5f,  0.5f}, glm::vec3(0.0f), glm::vec2(0.0f)),
        Vertex({ 0.5f, -0.5f,  0.5f}, glm::vec3(0.0f), glm::vec2(0.0f)),
        Vertex({-0.5f, -0.5f,  0.5f}, glm::vec3(0.0f), glm::vec2(0.0f)),
        Vertex({-0.5f,  0.5f, -0.5f}, glm::vec3(0.0f), glm::vec2(0.0f)),
        Vertex({ 0.5f,  0.5f, -0.5f}, glm::vec3(0.0f), glm::vec2(0.0f)),
        Vertex({ 0.5f, -0.5f, -0.5f}, glm::vec3(0.0f), glm::vec2(0.0f)),
        Vertex({-0.5f, -0.5f, -0.5f}, glm::vec3(0.0f), glm::vec2(0.0f)),
    };
    
    static void Initialize();
    
    void UpdateLookAtMatrix();
    void Update(glm::vec4 movement, float up, float down);
    
    glm::vec3 CalculateVelocity(glm::vec4 movement, float up, float down);
    glm::vec3 Step(glm::vec4 movement, float up, float down, float depth);
    
    std::vector<Vertex> GetColliderVertices();
    glm::mat4 CreateModelMatrix();
};

Camera camera;

void Camera::Initialize() {
    camera = Camera();
    
    camera.position = glm::vec3(0.0f, 6.0f, 0.0f);
    camera.lookDirection = glm::vec3(0.0f, 0.0f, -1.0f);
    
    camera.projection = glm::perspective(3.14159265358f/2.0f, 3.0f/2.0f, 0.1f, 1000.0f);
}

// Calculates and returns the velocity of the camera
glm::vec3 Camera::CalculateVelocity(glm::vec4 movement, float up, float down) {
    
    float forward   = movement.x, // motion inflicted by the W key
          backward  = movement.y, // motion inflicted by the S key
          left      = movement.z, // motion inflicted by the A key
          right     = movement.w; // motion inflicted by the D key
    
    // straight ahead motion (where the camera is pointing to)
    glm::vec3 motion = lookDirection;
    
    // motion perpendicular to the up vector and the motion vector
    glm::vec3 rightMotion = glm::normalize(glm::cross(motion, glm::vec3(0.0f, 1.0f, 0.0f)));
    
    // motions are the sum of the opposite possible directions
    // if at any time either the W and S keys are pressed at the same time, then forwardMotion is zero. the same goes for the others
    float forwardMotion  = forward + backward,
          sidewaysMotion = left + right,
          verticalMotion = up + down;
    
    // define each velocities
    glm::vec3 vStraight = motion * forwardMotion * speed;
    glm::vec3 vRight    = rightMotion * sidewaysMotion * speed;
    glm::vec3 vUp       = glm::vec3(0.0f, 1.0f, 0.0f) * verticalMotion * speed;
    
    return vStraight - vRight + vUp;
}

// Predicts the next camera position (useful for Continuous Collision Detection)
glm::vec3 Camera::Step(glm::vec4 movement, float up, float down, float depth = 1) {
    
    glm::vec3 velocity = CalculateVelocity(movement, up, down);
    
    return position + velocity * depth * core::deltaTime;
}

// Updates the camera (position, velocity, lookDirection)
void Camera::Update(glm::vec4 movement, float up, float down) {
        
    position = Step(movement, up, down);
    
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

std::vector<Vertex> Camera::GetColliderVertices() {
    
    glm::mat4 model = CreateModelMatrix();
    
    std::vector<Vertex> projectedVertices = std::vector<Vertex>();
    
    for (int i = 0; i < vertices.size(); i++) {
        glm::vec3 vertex = vertices[i].vertex;
        glm::vec3 projected = glm::vec3(model * glm::vec4(vertex, 1.0));
        projectedVertices.push_back(Vertex(projected, glm::vec3(0.0f), glm::vec2(0.0f)));
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

void Camera::UpdateLookAtMatrix() {
    lookAt = glm::lookAt(position, position + lookDirection, glm::vec3(0.0f, 1.0f, 0.0f));
}

}

#endif /* camera_h */
