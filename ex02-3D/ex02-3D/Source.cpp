#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <vector>

#include <GL\glew.h>
#include <GLFW\glfw3.h>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include "Mesh.h"
#include "Shader.h"
#include "Window.h"
#include "Camera.h"
#include "Texture.h"

std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;
Window mainWindow;
Camera camera;

Texture brickTexture;
Texture dirtTexture;

// Old implementation of FPS control
GLfloat deltaTime = 0.0f, lastTime = 0.0f;

static const char* vertexLocation = "Shaders/VertexShader.glsl";
static const char* fragmentLocation = "Shaders/FragmentShader.glsl";

// Function for creating a triangle (VAO and VBO)
void CreateObject() {
	GLfloat vertices[] = {
		 0.0f,  1.0f, 0.0f, 0.5f, 1.0f, // Vertex 0 (x, y, z, u, v)
		 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, // Vertex 1 (x, y, z, u, v)
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // Vertex 2 (x, y, z, u, v)
		 0.0f, -1.0f, 1.0f, 0.5f, 0.0f  // Vertex 3 (x, y, z, u, v)
	};

	unsigned int indices[]{
		0, 1, 2, // Pyramid front
		0, 1, 3, // Pyramid right
		0, 2, 3, // Pyramid left
		1, 2, 3  // Pyramid base
	};

	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, sizeof(vertices), sizeof(indices));
	meshList.push_back(obj1);

	Mesh* obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, sizeof(vertices), sizeof(indices));
	meshList.push_back(obj2);
}

void AddShader() {
	Shader* shader1 = new Shader();
	shader1->CreateFromFile(vertexLocation, fragmentLocation);
	shaderList.push_back(*shader1);
}

int main() {
	mainWindow = Window(800, 600);
	mainWindow.Initialize();

	// Create the objects
	CreateObject(); // Set the data in the GPU memory
	AddShader(); // Create and compile the shaders through the shader class

	// CAMERA
	//Args: (startPosition, startWorldUp, startYaw, startPitch, startMoveSpeed, startTurnSpeed)
	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 5.0f, 8.0f);

	// TEXTURES
	brickTexture = Texture((char*)"Textures/brick.png");
	brickTexture.loadTexture();
	dirtTexture = Texture((char*)"Textures/dirt.png");
	dirtTexture.loadTexture();

	// Calculate the 3D PROJECTION
	// Args: (real depth, display/window aspect ratio, virtual near clip depth, virtual far clip depth)
	glm::mat4 projection = glm::perspective(1.0f, mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);

	// Run till window gets closed
	while (!mainWindow.getWindowShouldClose()) {
		// Old implementation of FPS control
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		lastTime = now;

		// Activate inputs and events (mouse and keyboard input, for instance)
		glfwPollEvents();

		// Set up keyboard and mouse control
		camera.keyControl(mainWindow.getKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange(), deltaTime);

		/********************************
		*	Background Color
		*********************************/
		// Clear window and select a new color
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		// Load the selected color in the GPU memory buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // With the pipe operator both parameters are passed

		/********************************
		*	Use Shader
		*********************************/
		shaderList[0].UseProgram();

		// Set PROJECTION (Camera)
		// Updates the projection variable in the shader in order to multiply/transform our vertex matrix
		// Args: (projection, number of projections, should be transposed?, projection values)
		glUniformMatrix4fv(shaderList[0].getUniformProjection(), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(shaderList[0].getUniformView(), 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));

			/********************************
			*	Object 1
			*********************************/
			glm::mat4 model(1.0f); // Fill the (4x4) model matrix with 1's
			model = glm::translate(model, glm::vec3(0.0f, -0.25f, -2.5f)); // glm::vec3 returns the specified vector in the correct format
			model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
			//model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f)); 
			// Args: (shader model, number of matrices, should be transposed?, offset model values)
			glUniformMatrix4fv(shaderList[0].getUniformModel(), 1, GL_FALSE, glm::value_ptr(model)); // Assigns the new offset model to the shader uniModel
			brickTexture.useTexture(); // Uses the active texture in the buffer
			// Render object
			meshList[0]->RenderMesh(); 

			/********************************
			*	Object 2
			*********************************/
			model = glm::mat4(1.0f); // Creates a 4x4 matrix with 1.0f in every entry
			model = glm::translate(model, glm::vec3(0.0f, 0.75f, -2.5f));
			model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
			glUniformMatrix4fv(shaderList[0].getUniformModel(), 1, GL_FALSE, glm::value_ptr(model));
			dirtTexture.useTexture(); // Uses the active texture in the buffer
			// Render object
			meshList[1]->RenderMesh();

		glUseProgram(0); // Reset program pointer for the next program to be executed

		/********************************
		*	Update Screen
		*********************************/
		// Swap buffer -> Executes the instructions queued in the memory buffer
		mainWindow.swapBuffer();
	}

	return 0;
}