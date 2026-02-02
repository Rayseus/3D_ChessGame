#include "controls.hpp"

// View and projection matrices
namespace {
    glm::mat4 ViewMatrix;
    glm::mat4 ProjectionMatrix;

    // Camera parameters
    glm::vec3 position = glm::vec3(0, 0, 5);
    float horizontalAngle = 3.14f;    // Initial angle: toward -Z
    float verticalAngle = 0.0f;       // Initial vertical angle
    float initialFoV = 45.0f;         // Initial Field of View
    float radialDistance = 5.0f;      // Initial radial distance

    // Movement parameters
    const float SPEED = 6.0f;         // 3 units per second
    const float MOUSE_SPEED = 0.005f;

    // Window dimensions
    const int WINDOW_WIDTH = 1024;
    const int WINDOW_HEIGHT = 768;
}
bool lightEnabled = true;         // Initial light state

glm::mat4 getViewMatrix() {
    return ViewMatrix;
}

glm::mat4 getProjectionMatrix() {
    return ProjectionMatrix;
}

/**
 * @brief Handles keyboard input for camera movement
 * @param deltaTime Time elapsed since last frame
 */
void handleKeyboardInput(float deltaTime) {
    // Radial movement (W/S)
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        radialDistance -= deltaTime * SPEED;
        radialDistance = std::fmax(0.1f, radialDistance); // Prevent getting too close
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        radialDistance += deltaTime * SPEED;
    }

    // Horizontal rotation (A/D)
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        horizontalAngle -= deltaTime * SPEED;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        horizontalAngle += deltaTime * SPEED;
    }

    // Vertical rotation (Up/Down)
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        verticalAngle += deltaTime * SPEED;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        verticalAngle -= deltaTime * SPEED;
    }

    // Light toggle (L)
    static bool lastLState = false;
    bool currentLState = (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS);
    if (currentLState && !lastLState) {
        lightEnabled = !lightEnabled;
    }
    lastLState = currentLState;

    // Exit application (ESC)
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

/**
 * @brief Updates camera matrices based on user input
 * Handles both keyboard and mouse input to control camera position and orientation
 */
void computeMatricesFromInputs() {
    // Time management
    static double lastTime = glfwGetTime();
    double currentTime = glfwGetTime();
    float deltaTime = float(currentTime - lastTime);

    // Mouse position handling
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    glfwSetCursorPos(window, WINDOW_WIDTH/2, WINDOW_HEIGHT/2);

    // Update angles based on mouse movement
    horizontalAngle += MOUSE_SPEED * float(WINDOW_WIDTH/2 - xpos);
    verticalAngle   += MOUSE_SPEED * float(WINDOW_HEIGHT/2 - ypos);

    // Handle keyboard input
    handleKeyboardInput(deltaTime);

    // Convert spherical coordinates to Cartesian
    glm::vec3 direction(
        cos(verticalAngle) * sin(horizontalAngle),
        sin(verticalAngle),
        cos(verticalAngle) * cos(horizontalAngle)
    );

    // Calculate position based on radial distance
    position = radialDistance * direction;

    // Calculate right and up vectors
    glm::vec3 right(
        sin(horizontalAngle - glm::pi<float>()/2.0f),
        0,
        cos(horizontalAngle - glm::pi<float>()/2.0f)
    );
    glm::vec3 up = glm::cross(right, direction);

    // Update matrices
    float FoV = initialFoV;
    ProjectionMatrix = glm::perspective(glm::radians(FoV), 4.0f/3.0f, 0.1f, 100.0f);
    ViewMatrix = glm::lookAt(
        position,           // Camera position
        glm::vec3(0,0,0),  // Look at origin
        up                 // Up vector
    );

    lastTime = currentTime;
}