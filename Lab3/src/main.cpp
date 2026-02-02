/* Author: Ruiyang Li
Class: ECE6122
Last Date Modified: 10/26/2024
Description:
This project implements a 3D chess visualization system using OpenGL.
I have made significant modifications to multiple files to enhance functionality:

1. objloader.hpp/cpp:
   - Enhanced the model loading system
   - Added specialized chess piece loading functionality
   - Improved texture management for chess pieces

2. texture.cpp:
   - Modified texture loading for better chess piece appearance
   - Added Y-axis flipping functionality
   - Improved error handling in texture loading

3. controls.cpp/hpp:
   - Implemented spherical coordinate camera movement
   - Added lighting control functionality
   - Enhanced camera orientation handling

Key Model Modifications:
1. Board Model:
   - Optimized for triangle-based rendering
   - Enhanced texture mapping
   - Improved vertex organization

2. Chess Pieces:
   - Maintained proper scale relationships
   - Organized for efficient rendering
   - Enhanced texture coordination

All transformations, material properties, and lighting are handled programmatically.
The system provides smooth camera control and proper piece placement on the board.
*/

#include <stdio.h>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <common/shader.hpp>
#include <common/texture.hpp>
#include <common/controls.hpp>
#include <common/objloader.hpp>
#include <common/vboindexer.hpp>

GLFWwindow* window;

void render() {
	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(1024, 768, "ChessApplication", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window\n");
		getchar();
		glfwTerminate();
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
	}

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwPollEvents();
	glfwSetCursorPos(window, 1024/2, 768/2);

	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile shaders
	GLuint programID = LoadShaders("shaders/StandardShading.vertexshader", "shaders/StandardShading.fragmentshader");

	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
	GLuint ModelMatrixID = glGetUniformLocation(programID, "M");

	// Load textures
	GLuint boardTexture = loadBMP_custom("Stone_Chess_Board/12951_Stone_Chess_Board_diff.bmp");
	GLuint TextureID = glGetUniformLocation(programID, "myTextureSampler");

	// Load board model
	std::vector<glm::vec3> boardVertices;
	std::vector<glm::vec2> boardUvs;
	std::vector<glm::vec3> boardNormals;
	if (!loadOBJ("Stone_Chess_Board/12951_Stone_Chess_Board_v1_L3.obj",
				 boardVertices, boardUvs, boardNormals)) {
				 }

	std::vector<unsigned short> boardIndices;
	std::vector<glm::vec3> indexedBoardvertices;
	std::vector<glm::vec2> indexedBoarduvs;
	std::vector<glm::vec3> indexedBoardnormals;
	indexVBO(boardVertices, boardUvs, boardNormals,
			 boardIndices, indexedBoardvertices, indexedBoarduvs, indexedBoardnormals);

	// Setup board buffers
	GLuint boardVertexbuffer;
	glGenBuffers(1, &boardVertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, boardVertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexedBoardvertices.size() * sizeof(glm::vec3),
				 &indexedBoardvertices[0], GL_STATIC_DRAW);
	GLuint boardUvbuffer;
	glGenBuffers(1, &boardUvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, boardUvbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexedBoarduvs.size() * sizeof(glm::vec2),
				 &indexedBoarduvs[0], GL_STATIC_DRAW);

	GLuint boardNormalbuffer;
	glGenBuffers(1, &boardNormalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, boardNormalbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexedBoardnormals.size() * sizeof(glm::vec3),
				 &indexedBoardnormals[0], GL_STATIC_DRAW);

	GLuint boardElementbuffer;
	glGenBuffers(1, &boardElementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boardElementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, boardIndices.size() * sizeof(unsigned short),
				 &boardIndices[0], GL_STATIC_DRAW);

	std::vector<ChessPiece> chessPieces;
	// Load chess pieces
	if (!loadAssImp("Chess/chess.obj", chessPieces)) {
		fprintf(stderr, "Failed to load chess pieces.\n");
	}

	// Setup chess piece buffers
	for (ChessPiece& piece : chessPieces) {
		piece.setBuffers();
	}

	// Setup lighting
	glUseProgram(programID);
	GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");
	GLuint lightEnableID = glGetUniformLocation(programID, "enableLight");

	double lastTime = glfwGetTime();
	int nbFrames = 0;

	do {
		double currentTime = glfwGetTime();
		nbFrames++;
		if (currentTime - lastTime >= 1.0) {
			printf("%f ms/frame\n", 1000.0/double(nbFrames));
			nbFrames = 0;
			lastTime += 1.0;
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		computeMatricesFromInputs();

	    glm::mat4 ProjectionMatrix = getProjectionMatrix();
	    glm::mat4 ViewMatrix = getViewMatrix();

	    glUseProgram(programID);

	    glm::vec3 lightPos = glm::vec3(0, 25, 0);
	    glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
	    glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);
	    glUniform1i(lightEnableID, lightEnabled);

	    glm::mat4 ModelMatrix = glm::mat4(1.0);
	    ModelMatrix = glm::rotate(ModelMatrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	    ModelMatrix = glm::translate(ModelMatrix, glm::vec3(0.0f, 0.0f, 0.5f));

	    glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
	    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
	    glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);

	    // Bind texture
	    glActiveTexture(GL_TEXTURE0);
	    glBindTexture(GL_TEXTURE_2D, boardTexture);
	    glUniform1i(TextureID, 0);

	    // Set up vertex attributes
	    glEnableVertexAttribArray(0);
	    glBindBuffer(GL_ARRAY_BUFFER, boardVertexbuffer);
	    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	    glEnableVertexAttribArray(1);
	    glBindBuffer(GL_ARRAY_BUFFER, boardUvbuffer);
	    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	    glEnableVertexAttribArray(2);
	    glBindBuffer(GL_ARRAY_BUFFER, boardNormalbuffer);
	    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	    // Draw board
	    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boardElementbuffer);
	    glDrawElements(GL_TRIANGLES, boardIndices.size(), GL_UNSIGNED_SHORT, (void*)0);

		float spacing = 5.5f;

	    // Reset all piece matrices
	    for (ChessPiece& piece : chessPieces) {
	        piece.ModelMatrix = glm::mat4(1.0f);
	    }

	    // Pawn
		for (int col = 0; col < 8; col++)
		{
			chessPieces[5].ModelMatrix = glm::mat4(1.0f);
	        chessPieces[5].ModelMatrix = glm::translate(chessPieces[5].ModelMatrix, glm::vec3((col - 1) * spacing, 0.0f, 30.0f));
			chessPieces[5].ModelMatrix = glm::translate(chessPieces[5].ModelMatrix, glm::vec3(-27.3f, 0.0f, 0.0f));

	        chessPieces[5].render(programID, MatrixID, ViewMatrixID, ModelMatrixID,
	                          ProjectionMatrix, ViewMatrix);
		}
		// Knight
		for (int i = 0; i <=1; i++)
		{
			chessPieces[3].ModelMatrix = glm::mat4(1.0f);
			chessPieces[3].ModelMatrix = glm::translate(chessPieces[3].ModelMatrix, glm::vec3((i-1) * spacing * 5, 0.0f, 25.0f));
			chessPieces[3].ModelMatrix = glm::translate(chessPieces[3].ModelMatrix, glm::vec3(22.0f, 0.0f, 0.0f));

			chessPieces[3].render(programID, MatrixID, ViewMatrixID, ModelMatrixID,
	                          ProjectionMatrix, ViewMatrix);
		}

		// Bishop
		for (int i = 0; i <=1; i++)
		{
			chessPieces[1].ModelMatrix = glm::mat4(1.0f);
			chessPieces[1].ModelMatrix = glm::translate(chessPieces[1].ModelMatrix, glm::vec3((i-1) * spacing * 3, 0.0f, 25.0f));
			chessPieces[1].ModelMatrix = glm::translate(chessPieces[1].ModelMatrix, glm::vec3(11.5f, 0.0f, 0.0f));
			chessPieces[1].render(programID, MatrixID, ViewMatrixID, ModelMatrixID,
	                          ProjectionMatrix, ViewMatrix);
		}
		// Rook
		for (int i = 0; i <=1; i++)
		{
			chessPieces[11].ModelMatrix = glm::mat4(1.0f);
			chessPieces[11].ModelMatrix = glm::translate(chessPieces[11].ModelMatrix, glm::vec3((i-1) * spacing * 7, 0.0f, 25.0f));
			chessPieces[11].ModelMatrix = glm::translate(chessPieces[11].ModelMatrix, glm::vec3(34.0f, 0.0f, 0.0f));

			chessPieces[11].render(programID, MatrixID, ViewMatrixID, ModelMatrixID,
	                          ProjectionMatrix, ViewMatrix);
		}
		// King
		chessPieces[9].ModelMatrix = glm::mat4(1.0f);
		chessPieces[9].ModelMatrix = glm::translate(chessPieces[9].ModelMatrix, glm::vec3(0.0f, 0.0f, 25.0f));

		chessPieces[9].render(programID, MatrixID, ViewMatrixID, ModelMatrixID,
	                          ProjectionMatrix, ViewMatrix);
		// Queen
		chessPieces[7].ModelMatrix = glm::mat4(1.0f);
		chessPieces[7].ModelMatrix = glm::translate(chessPieces[7].ModelMatrix, glm::vec3(0.0f, 0.0f, 25.0f));
		chessPieces[7].ModelMatrix = glm::translate(chessPieces[7].ModelMatrix, glm::vec3(-11.0f, 0.0f, 0.0f));
		chessPieces[7].render(programID, MatrixID, ViewMatrixID, ModelMatrixID,
	                          ProjectionMatrix, ViewMatrix);


	    // Setup black pieces
	    float currentX = 5.0f;

	    // Pawn
		for (int col = 0; col < 8; col++)
		{
			chessPieces[4].ModelMatrix = glm::mat4(1.0f);
	        chessPieces[4].ModelMatrix = glm::translate(chessPieces[4].ModelMatrix, glm::vec3(currentX + (col - 1) * spacing, 0.0f, 45.0f));
			chessPieces[4].ModelMatrix = glm::translate(chessPieces[4].ModelMatrix, glm::vec3(-32.0f, 0.0f, 0.0f));

	        chessPieces[4].render(programID, MatrixID, ViewMatrixID, ModelMatrixID,
	                          ProjectionMatrix, ViewMatrix);
		}
		// Knight
		for (int i = 0; i <=1; i++)
		{
			chessPieces[2].ModelMatrix = glm::mat4(1.0f);
			chessPieces[2].ModelMatrix = glm::translate(chessPieces[2].ModelMatrix, glm::vec3((i-1) * spacing * 5, 0.0f, 0.0f));
			chessPieces[2].ModelMatrix = glm::translate(chessPieces[2].ModelMatrix, glm::vec3(5.3f, 0.0f, 0.0f));
			chessPieces[2].ModelMatrix = glm::translate(chessPieces[2].ModelMatrix, glm::vec3(0.0f, 0.0f, -12.0f));
			chessPieces[2].ModelMatrix = glm::rotate(chessPieces[2].ModelMatrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

			chessPieces[2].render(programID, MatrixID, ViewMatrixID, ModelMatrixID,
	                          ProjectionMatrix, ViewMatrix);
		}
		// Bishop
		for (int i = 0; i <=1; i++)
		{
			chessPieces[0].ModelMatrix = glm::mat4(1.0f);
			chessPieces[0].ModelMatrix = glm::translate(chessPieces[0].ModelMatrix, glm::vec3((i-1) * spacing * 3, 0.0f, 0.0f));
			chessPieces[0].ModelMatrix = glm::translate(chessPieces[0].ModelMatrix, glm::vec3(5.3f, 0.0f, 0.0f));
			chessPieces[0].ModelMatrix = glm::translate(chessPieces[0].ModelMatrix, glm::vec3(0.0f, 0.0f, -12.0f));
			chessPieces[0].ModelMatrix = glm::rotate(chessPieces[0].ModelMatrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

			chessPieces[0].render(programID, MatrixID, ViewMatrixID, ModelMatrixID,
	                          ProjectionMatrix, ViewMatrix);
		}
		// Rook
		for (int i = 0; i <=1; i++)
		{
			chessPieces[10].ModelMatrix = glm::mat4(1.0f);
			chessPieces[10].ModelMatrix = glm::translate(chessPieces[10].ModelMatrix, glm::vec3((i-1) * spacing * 7, 0.0f, 0.0f));
			chessPieces[10].ModelMatrix = glm::translate(chessPieces[10].ModelMatrix, glm::vec3(5.3f, 0.0f, 0.0f));
			chessPieces[10].ModelMatrix = glm::translate(chessPieces[10].ModelMatrix, glm::vec3(0.0f, 0.0f, -12.0f));
			chessPieces[10].ModelMatrix = glm::rotate(chessPieces[10].ModelMatrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

			chessPieces[10].render(programID, MatrixID, ViewMatrixID, ModelMatrixID,
	                          ProjectionMatrix, ViewMatrix);
		}
		// King
		chessPieces[8].ModelMatrix = glm::mat4(1.0f);
		chessPieces[8].ModelMatrix = glm::translate(chessPieces[8].ModelMatrix, glm::vec3(5.3f, 0.0f, 0.0f));
		chessPieces[8].ModelMatrix = glm::translate(chessPieces[8].ModelMatrix, glm::vec3(0.0f, 0.0f, -12.0f));
		chessPieces[8].ModelMatrix = glm::rotate(chessPieces[8].ModelMatrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		chessPieces[8].render(programID, MatrixID, ViewMatrixID, ModelMatrixID,
	                          ProjectionMatrix, ViewMatrix);
		// Queen
		chessPieces[6].ModelMatrix = glm::mat4(1.0f);
		chessPieces[6].ModelMatrix = glm::translate(chessPieces[6].ModelMatrix, glm::vec3(5.3f, 0.0f, 0.0f));
		chessPieces[6].ModelMatrix = glm::translate(chessPieces[6].ModelMatrix, glm::vec3(0.0f, 0.0f, -12.0f));
		chessPieces[6].ModelMatrix = glm::rotate(chessPieces[6].ModelMatrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		chessPieces[6].render(programID, MatrixID, ViewMatrixID, ModelMatrixID,
	                          ProjectionMatrix, ViewMatrix);
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);

		glfwSwapBuffers(window);
		glfwPollEvents();
	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	glDeleteBuffers(1, &boardVertexbuffer);
	glDeleteBuffers(1, &boardUvbuffer);
	glDeleteBuffers(1, &boardNormalbuffer);
	glDeleteBuffers(1, &boardElementbuffer);
	glDeleteProgram(programID);
	glDeleteTextures(1, &boardTexture);
	glDeleteVertexArrays(1, &VertexArrayID);

	glfwTerminate();
}

int main() {
	render();
	return 0;
}