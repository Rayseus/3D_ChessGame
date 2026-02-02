/* Author: Ruiyang Li
Class: ECE6122
Last Date Modified: 10/26/2024
Description:
Header file for the objloader with improvements to class definitions and documentation.
Key changes include:
- Enhanced class documentation and member descriptions
- Better organized class structure
- Added support for improved texture handling
- Clear separation of responsibilities
- Added proper memory management support
*/

#ifndef OBJLOADER_H
#define OBJLOADER_H

#include <vector>
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

/**
 * @brief ChessPiece class represents a 3D chess piece in a scene.
 *
 * This class encapsulates the geometry, texture, and transformation data for a chess piece,
 * providing methods for rendering and manipulation in 3D space. Each piece maintains its own
 * vertex data, texture coordinates, and OpenGL buffers.
 */
class ChessPiece {
public:
    // Geometry data
    std::vector<glm::vec3> vertices;  ///< Vertex positions
    std::vector<glm::vec2> uvs;       ///< Texture coordinates
    std::vector<glm::vec3> normals;   ///< Normal vectors
    std::vector<unsigned short> indices; ///< Face indices

    // OpenGL handles
    GLuint textureID;     ///< OpenGL texture identifier
    GLuint vertexbuffer;  ///< Vertex buffer object
    GLuint uvbuffer;      ///< UV coordinates buffer
    GLuint normalbuffer;  ///< Normal vectors buffer
    GLuint elementbuffer; ///< Element/Index buffer

    // Transform data
    glm::mat4 ModelMatrix; ///< Model transformation matrix

    // Texture information
    std::string textureFile; ///< Path to texture file

    /**
     * @brief Default constructor initializing the model matrix
     */
    ChessPiece() : ModelMatrix(glm::mat4(1.0f)) {}

    /**
     * @brief Copy constructor for deep copying of chess piece data
     */
    ChessPiece(const ChessPiece& other)
        : vertices(other.vertices)
        , uvs(other.uvs)
        , normals(other.normals)
        , indices(other.indices)
        , textureID(other.textureID)
        , ModelMatrix(other.ModelMatrix) {}

    /**
     * @brief Assignment operator for deep copying of chess piece data
     */
    ChessPiece& operator=(const ChessPiece& other) {
        if (this != &other) {
            vertices = other.vertices;
            uvs = other.uvs;
            normals = other.normals;
            indices = other.indices;
            textureID = other.textureID;
            ModelMatrix = other.ModelMatrix;
        }
        return *this;
    }

    /**
     * @brief Renders the chess piece using OpenGL
     *
     * @param programID Shader program ID
     * @param MatrixID MVP matrix uniform location
     * @param ViewMatrixID View matrix uniform location
     * @param ModelMatrixID Model matrix uniform location
     * @param ProjectionMatrix Camera projection matrix
     * @param ViewMatrix Camera view matrix
     */
    void render(GLuint programID, GLuint MatrixID, GLuint ViewMatrixID,
                GLuint ModelMatrixID, const glm::mat4& ProjectionMatrix,
                const glm::mat4& ViewMatrix);

    /**
     * @brief Sets up OpenGL buffers for the chess piece
     */
    void setBuffers();

    /**
     * @brief Rotates the chess piece around a specified axis
     *
     * @param angle Rotation angle in degrees
     * @param axis Rotation axis vector
     */
    void rotate(float angle, glm::vec3 axis) {
        ModelMatrix = glm::rotate(ModelMatrix, glm::radians(angle), axis);
    }

    /**
     * @brief Translates the chess piece by a specified vector
     *
     * @param translation Translation vector
     */
    void translate(glm::vec3 translation) {
        ModelMatrix = glm::translate(ModelMatrix, translation);
    }
};

/**
 * @brief Loads a model from an OBJ file
 *
 * @param path File path to the OBJ model
 * @param out_vertices Output vector for vertex positions
 * @param out_uvs Output vector for texture coordinates
 * @param out_normals Output vector for normal vectors
 * @return bool Success status of the loading operation
 */
bool loadOBJ(
    const char* path,
    std::vector<glm::vec3>& out_vertices,
    std::vector<glm::vec2>& out_uvs,
    std::vector<glm::vec3>& out_normals
);

/**
 * @brief Loads a 3D chess model using Assimp library
 *
 * @param path Path to the model file
 * @param chessPieces Vector to store the loaded chess pieces
 * @return bool Success status of the loading operation
 */
bool loadAssImp(
    const char* path,
    std::vector<ChessPiece>& chessPieces
);

#endif