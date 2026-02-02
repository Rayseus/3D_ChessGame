/* Author: Ruiyang Li
Class: ECE6122
Last Date Modified: 10/26/2024
Description:
Header file for texture loading functionality with improvements including:
- Better function documentation
- Clear separation of texture loading variants
- Improved naming for specialized functions
*/

#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <GL/glew.h>

/**
 * @brief Flips texture data vertically along the Y-axis
 *
 * @param data Pointer to the texture data
 * @param width Width of the texture in pixels
 * @param height Height of the texture in pixels
 */
void flipTextureY(unsigned char* data, int width, int height);

/**
 * @brief Loads a BMP file and creates an OpenGL texture with Y-flipping
 * Used for standard texture loading where Y-flipping is needed
 *
 * @param imagepath Path to the BMP file
 * @return GLuint OpenGL texture identifier, 0 if loading fails
 */
GLuint loadBMP_custom(const char* imagepath);

/**
 * @brief Loads a BMP file for chess pieces without Y-flipping
 * Specialized version for chess piece textures where Y-flipping is not needed
 *
 * @param imagepath Path to the BMP file
 * @return GLuint OpenGL texture identifier, 0 if loading fails
 */
GLuint loadChessTexture(const char* imagepath);

/**
 * @brief Loads a DDS file and creates an OpenGL texture
 * Supports DXT1, DXT3, and DXT5 compression formats
 *
 * @param imagepath Path to the DDS file
 * @return GLuint OpenGL texture identifier, 0 if loading fails
 */
GLuint loadDDS(const char* imagepath);

// Note: GLFW's texture loading functionality has been removed since GLFW 3
// Alternative implementations are provided above

#endif