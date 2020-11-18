#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <vector>

#include <GL\glew.h>
#include <GLFW\glfw3.h>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include "CommonValues.h"
#include "Mesh.h"
#include "Shader.h"
#include "Window.h"
#include "Camera.h"
#include "Texture.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"

std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;
Window mainWindow;
Camera camera;

DirectionalLight mainLight;
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

Material metalMaterial;
Material woodMaterial;

Texture brickTexture;
Texture dirtTexture;

// Old implementation of FPS control
GLfloat deltaTime = 0.0f, lastTime = 0.0f;

static const char* vertexLocation = "Shaders/VertexShader.glsl";
static const char* fragmentLocation = "Shaders/FragmentShader.glsl";

// Normal calculations (source: OpenGL)
void CalcAverageNormal(unsigned int* indices, unsigned int indexCount, GLfloat* vertices, unsigned int vertexCount,
					unsigned int vLength, unsigned int normalOffset) {
	for (int i=0; i < indexCount; i=i+3) {
		unsigned int i0 = indices[i] * vLength;
		unsigned int i1 = indices[i+1] * vLength;
		unsigned int i2 = indices[i+2] * vLength;

		glm::vec3 v1(vertices[i1] - vertices[i0], vertices[i1 + 1] - vertices[i0 + 1], vertices[i1 + 2] - vertices[i0 + 2]);
		glm::vec3 v2(vertices[i2] - vertices[i0], vertices[i2 + 1] - vertices[i0 + 1], vertices[i2 + 2] - vertices[i0 + 2]);
		glm::vec3 normal = glm::normalize(glm::cross(v1, v2));

		vertices[i0 + normalOffset] += normal.x; vertices[i0 + 1 + normalOffset] += normal.y; vertices[i0 + 2 + normalOffset] += normal.z;
		vertices[i1 + normalOffset] += normal.x; vertices[i1 + 1 + normalOffset] += normal.y; vertices[i1 + 2 + normalOffset] += normal.z;
		vertices[i2 + normalOffset] += normal.x; vertices[i2 + 1 + normalOffset] += normal.y; vertices[i2 + 2 + normalOffset] += normal.z;
	}

	for (int i=0; i < vertexCount / vLength; i++) {
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x;
		vertices[nOffset+1] = vec.y;
		vertices[nOffset+2] = vec.z;
	}
}


// Function for creating a triangle (VAO and VBO)
void CreateObject() {
	GLfloat vertices[] = {
		 0.0f,  1.0f,  0.0f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, // Vertex 0 (x, y, z, u, v, nx, ny, nz)
		 1.0f, -1.0f, -0.6f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // Vertex 1 (x, y, z, u, v, nx, ny, nz)
		-1.0f, -1.0f, -0.6f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, // Vertex 2 (x, y, z, u, v, nx, ny, nz)
		 0.0f, -1.0f,  1.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f  // Vertex 3 (x, y, z, u, v, nx, ny, nz)
	};

	unsigned int indices[]{
		0, 1, 2, // Pyramid front
		0, 1, 3, // Pyramid right
		0, 2, 3, // Pyramid left
		1, 2, 3  // Pyramid base
	};

	GLfloat floorVertices[] = {
		-10.0, 0.0f, -10.0f,  0.0f,  0.0f, 0.0f, -1.0f, 0.0f,
		 10.0, 0.0f, -10.0f, 10.0f,  0.0f, 0.0f, -1.0f, 0.0f,
		-10.0, 0.0f,  10.0f,  0.0f, 10.0f, 0.0f, -1.0f, 0.0f,
		 10.0, 0.0f,  10.0f, 10.0f, 10.0f, 0.0f, -1.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	// Calculate the normals
	CalcAverageNormal(indices, 12, vertices, 32, 8, 5);

	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh* obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh* obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);
}

void AddShader() {
	Shader* shader1 = new Shader();
	shader1->CreateFromFile(vertexLocation, fragmentLocation);
	shaderList.push_back(*shader1);
}

int main() {
	mainWindow = Window(1280, 720);
	mainWindow.Initialize();

	// Create the objects
	CreateObject(); // Set the data in the GPU memory
	AddShader(); // Create and compile the shaders through the shader class

	// CAMERA
	//Args: (startPosition, startWorldUp, startYaw, startPitch, startMoveSpeed, startTurnSpeed)
	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 5.0f, 8.0f);

	// MATERIAL
	metalMaterial = Material(1.0f, 32.0f);
	woodMaterial = Material(0.3f, 4.0f);

	// DIRECTIONAL LIGHT
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,		// RGB
								 0.3f, 0.4f,			// ambient | diffuse intensities
					            -8.0f, -8.0f, -2.0f);	// x, y, z

	// POINT LIGHTS
	unsigned int pointLightsCount = 0;
	pointLights[0] = PointLight(0.0f, 1.0f, 1.0f,		// RGB
							    0.3f, 1.0f,				// ambient | diffuse intensities
							   -2.0f, 1.0f, -2.0f,		// Position (x, y, z)
							    0.3f, 0.2f, 0.1);		// constant, linear, exponent
	pointLightsCount++;

	pointLights[1] = PointLight(0.0f, 0.0f, 1.0f,		// RGB
							    0.3f, 1.0f,				// ambient | diffuse intensities
							    2.0f, 1.0f, -2.0f,		// Position (x, y, z)
								0.3f, 0.2f, 0.1f);		// constant, linear, exponent
	pointLightsCount++;

	// SPOT LIGHTS
	unsigned int spotLightsCount = 0;
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,				// RGB
						      0.5f, 0.5f,					// ambient | diffuse intensities
						      0.0f, 1.0f, 5.0f,				// Position (x, y, z)
					          0.0f, -1.0f, 0.0f,			// Direction (x, y, z)
						      0.3f, 0.2f, 0.1f, 20.0f);		// constant, linear, exponent, edge
	spotLightsCount++;

	// TEXTURES
	brickTexture = Texture((char*)"Textures/brick.png");
	brickTexture.loadTexture();
	dirtTexture = Texture((char*)"Textures/dirt.png");
	dirtTexture.loadTexture();

	// Calculate the 3D PROJECTION
	// Args: (fovy, display/window aspect ratio, virtual near clip depth, virtual far clip depth)
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);

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
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
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
			*	Lights
			*********************************/	
			shaderList[0].setDirectionalLight(&mainLight);
			//shaderList[0].setPointLight(pointLights, pointLightsCount);
			// Update flashlight position
			spotLights[0].SetFlash(camera.getCameraPosition(), camera.getCameraDirection());
			shaderList[0].setSpotLight(spotLights, spotLightsCount);

			/********************************
			*	Object 1
			*********************************/
			glm::mat4 model(1.0f); // Fill the (4x4) model matrix with 1's
			model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.5f)); // glm::vec3 returns the specified vector in the correct format
			//model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
			//model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f)); 
			// Args: (shader model, number of matrices, should be transposed?, offset model values)
			glUniformMatrix4fv(shaderList[0].getUniformModel(), 1, GL_FALSE, glm::value_ptr(model)); // Assigns the new offset model to the shader uniModel
			brickTexture.useTexture(); // Uses the active texture in the buffer
			metalMaterial.useMaterial(shaderList[0].getUniformSpecularIntensity(), shaderList[0].getUniformShininess());
			// Render object
			meshList[0]->RenderMesh(); 

			/********************************
			*	Object 2
			*********************************/
			model = glm::mat4(1.0f); // Creates a 4x4 matrix with 1.0f in every entry
			model = glm::translate(model, glm::vec3(0.0f, 4.0f, -2.5f));
			//model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
			glUniformMatrix4fv(shaderList[0].getUniformModel(), 1, GL_FALSE, glm::value_ptr(model));
			brickTexture.useTexture(); // Uses the active texture in the buffer
			metalMaterial.useMaterial(shaderList[0].getUniformSpecularIntensity(), shaderList[0].getUniformShininess());
			// Render object 
			meshList[1]->RenderMesh();

			/********************************
			*	Object 3 FLOOR
			*********************************/
			model = glm::mat4(1.0f); // Creates a 4x4 matrix with 1.0f in every entry
			model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
			glUniformMatrix4fv(shaderList[0].getUniformModel(), 1, GL_FALSE, glm::value_ptr(model));
			dirtTexture.useTexture(); // Uses the active texture in the buffer
			woodMaterial.useMaterial(shaderList[0].getUniformSpecularIntensity(), shaderList[0].getUniformShininess());
			// Render object 
			meshList[2]->RenderMesh();

		glUseProgram(0); // Reset program pointer for the next program to be executed

		/********************************
		*	Update Screen
		*********************************/
		// Swap buffer -> Executes the instructions queued in the memory buffer
		mainWindow.swapBuffer();
	}

	return 0;
}