/* Author: Ruiyang Li
Class: ECE6122
Last Date Modified: 10/26/2024
Description:
Camera control system with improvements including:
- Enhanced spherical coordinate camera movement
- Added lighting control functionality
- Improved camera orientation handling
- Better organized code structure and documentation
*/

#ifndef CONTROLS_HPP
#define CONTROLS_HPP

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Forward declaration of window to avoid circular dependency
extern GLFWwindow* window;

/**
 * @brief Gets the current view matrix
 * @return Current view matrix
 */
glm::mat4 getViewMatrix();

/**
 * @brief Gets the current projection matrix
 * @return Current projection matrix
 */
glm::mat4 getProjectionMatrix();

/**
 * @brief Updates camera matrices based on user input
 * Handles keyboard and mouse input to control camera position and orientation
 */
void computeMatricesFromInputs();
extern bool lightEnabled;

#endif