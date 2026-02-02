/* Author: Ruiyang Li
Class: ECE6122
Last Date Modified: 10/26/2024
Description:
This file contains the original objloader and includes improvements to the loadAssImp, render and setBuffers function.
Key changes include:
- Enhanced error handling and validation in model loading
- Improved performance with pre-allocated vectors
- Better organized mesh and texture processing
- Added detailed documentation and clear code structure
- More efficient memory management with move semantics
The core functionality remains the same while providing better reliability and maintainability.
*/

#include <vector>
#include <stdio.h>
#include <string>
#include <cstring>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include "objloader.hpp"
#include "texture.hpp"

// Very, VERY simple OBJ loader.
// Here is a short list of features a real function would provide : 
// - Binary files. Reading a model should be just a few memcpy's away, not parsing a file at runtime. In short : OBJ is not very great.
// - Animations & bones (includes bones weights)
// - Multiple UVs
// - All attributes should be optional, not "forced"
// - More stable. Change a line in the OBJ file and it crashes.
// - More secure. Change another line and you can inject code.
// - Loading from memory, stream, etc


bool loadOBJ(
	const char * path, 
	std::vector<glm::vec3> & out_vertices, 
	std::vector<glm::vec2> & out_uvs,
	std::vector<glm::vec3> & out_normals
){
	printf("Loading OBJ file %s...\n", path);

	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> temp_vertices; 
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;


	FILE * file = fopen(path, "r");
	if( file == NULL ){
		printf("Impossible to open the file ! Are you in the right path ? See Tutorial 1 for details\n");
		getchar();
		return false;
	}

	while( 1 ){

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		// else : parse lineHeader
		
		if ( strcmp( lineHeader, "v" ) == 0 ){
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
			temp_vertices.push_back(vertex);
		}else if ( strcmp( lineHeader, "vt" ) == 0 ){
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y );
			uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
			temp_uvs.push_back(uv);
		}else if ( strcmp( lineHeader, "vn" ) == 0 ){
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
			temp_normals.push_back(normal);
		}else if ( strcmp( lineHeader, "f" ) == 0 ){
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
			if (matches != 9){
				printf("File can't be read by our simple parser :-( Try exporting with other options\n");
				fclose(file);
				return false;
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices    .push_back(uvIndex[0]);
			uvIndices    .push_back(uvIndex[1]);
			uvIndices    .push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}else{
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}

	}

	// For each vertex of each triangle
	for( unsigned int i=0; i<vertexIndices.size(); i++ ){

		// Get the indices of its attributes
		unsigned int vertexIndex = vertexIndices[i];
		unsigned int uvIndex = uvIndices[i];
		unsigned int normalIndex = normalIndices[i];
		
		// Get the attributes thanks to the index
		glm::vec3 vertex = temp_vertices[ vertexIndex-1 ];
		glm::vec2 uv = temp_uvs[ uvIndex-1 ];
		glm::vec3 normal = temp_normals[ normalIndex-1 ];
		
		// Put the attributes in buffers
		out_vertices.push_back(vertex);
		out_uvs     .push_back(uv);
		out_normals .push_back(normal);
	
	}
	fclose(file);
	return true;
}

/**
 * @brief Loads a 3D chess model and creates chess pieces
 * @param path Path to the model file
 * @param chessPieces Vector to store the loaded chess pieces
 * @return Success status
 */
bool loadAssImp(const char* path, std::vector<ChessPiece>& chessPieces) {
    // Initialize Assimp importer
    Assimp::Importer importer;

    // Set import flags for optimal model loading
    unsigned int importFlags = aiProcess_Triangulate |
                             aiProcess_JoinIdenticalVertices |
                             aiProcess_SortByPType;

    // Load the 3D scene
    const aiScene* scene = importer.ReadFile(path, importFlags);
    if (!scene) {
        fprintf(stderr, "Error loading model: %s\n", importer.GetErrorString());
        return false;
    }

    // Define texture paths
	std::vector<const char*> textureFiles;
		textureFiles.push_back("Chess/woodlig3.bmp");
		textureFiles.push_back("Chess/wooddar3.bmp");
		textureFiles.push_back("Chess/woodlig2.bmp");
		textureFiles.push_back("Chess/wooddar2.bmp");
		textureFiles.push_back("Chess/woodlig0.bmp");
		textureFiles.push_back("Chess/wooddar0.bmp");
		textureFiles.push_back("Chess/woodlig5.bmp");
		textureFiles.push_back("Chess/wooddar5.bmp");
		textureFiles.push_back("Chess/woodlig4.bmp");
		textureFiles.push_back("Chess/wooddar4.bmp");
		textureFiles.push_back("Chess/woodlig1.bmp");
		textureFiles.push_back("Chess/wooddar1.bmp");

    // Reserve space for chess pieces to avoid reallocations
    chessPieces.reserve(scene->mNumMeshes);

    // Process each mesh in the scene
    for (unsigned int meshIndex = 0; meshIndex < scene->mNumMeshes; meshIndex++) {
        const aiMesh* mesh = scene->mMeshes[meshIndex];
        ChessPiece piece;

        // Pre-allocate vectors to avoid reallocations
        piece.vertices.reserve(mesh->mNumVertices);
        piece.uvs.reserve(mesh->mNumVertices);
        piece.normals.reserve(mesh->mNumVertices);
        piece.indices.reserve(mesh->mNumFaces * 3);

        // Process vertices, UVs, and normals
        for (unsigned int vertexIndex = 0; vertexIndex < mesh->mNumVertices; vertexIndex++) {
            // Load vertex positions
            const aiVector3D& pos = mesh->mVertices[vertexIndex];
            piece.vertices.push_back(glm::vec3(pos.x, pos.y, pos.z));

            // Load texture coordinates (UVs)
            if (mesh->HasTextureCoords(0)) {
                const aiVector3D& uv = mesh->mTextureCoords[0][vertexIndex];
                piece.uvs.push_back(glm::vec2(uv.x, uv.y));
            }

            // Load normals
            if (mesh->HasNormals()) {
                const aiVector3D& normal = mesh->mNormals[vertexIndex];
                piece.normals.push_back(glm::vec3(normal.x, normal.y, normal.z));
            }
        }

        // Process face indices
        for (unsigned int faceIndex = 0; faceIndex < mesh->mNumFaces; faceIndex++) {
            const aiFace& face = mesh->mFaces[faceIndex];
            for (unsigned int indexIndex = 0; indexIndex < 3; indexIndex++) {
                piece.indices.push_back(face.mIndices[indexIndex]);
            }
        }

        // Load and assign texture
        if (meshIndex < textureFiles.size()) {
            piece.textureID = loadChessTexture(textureFiles[meshIndex]);
        } else {
            fprintf(stderr, "Warning: No texture file defined for mesh %d\n", meshIndex);
            piece.textureID = 0; // Use default texture ID
        }

        // Add the piece to the collection
        chessPieces.push_back(std::move(piece));
    }

    return true;
}

/**
 * @brief Renders the chess piece using OpenGL.
 * 
 * @param programID ID of the shader program.
 * @param MatrixID ID of the MVP matrix uniform.
 * @param ViewMatrixID ID of the View matrix uniform.
 * @param ModelMatrixID ID of the Model matrix uniform.
 * @param ProjectionMatrix Projection matrix.
 * @param ViewMatrix View matrix.
 * @return void
 * 
 * This function renders the chess piece by setting the MVP matrix, binding the texture, 
 * sending vertex, UV, and normal data to the GPU, and drawing the indexed elements. 
 * After rendering, it disables the vertex attributes.
 */
void ChessPiece::render(GLuint programID, GLuint MatrixID, GLuint ViewMatrixID, GLuint ModelMatrixID, 
                const glm::mat4& ProjectionMatrix, const glm::mat4& ViewMatrix)
{
	glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
    // Bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(glGetUniformLocation(programID, "myTextureSampler"), 0);
    // Bind vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    // Bind UVs
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    // Bind normals
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    // Bind index buffer and draw
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, (void*)0);
    // Disable attributes
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

/**
 * @brief Sets up the OpenGL buffers for the chess piece.
 * 
 * @return void
 * 
 * This function generates and binds vertex buffer objects (VBOs) for the vertices, UVs, normals, 
 * and element indices of the chess piece, and uploads the data to the GPU using `glBufferData`.
 */
void ChessPiece::setBuffers()
{
	// Setup VBO and Element buffer
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
    glGenBuffers(1, &uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
    glGenBuffers(1, &elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);

}
