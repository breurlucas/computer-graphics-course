#include <stdio.h>
#include <string.h>

#include <GL\glew.h>
#include <GLFW\glfw3.h>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include <iostream>
#include <chrono>

// By using the OpenGL types (like GLint) GLEW ensures we are using the most optimized implementations (memory allocation, for instance)
const GLint WIDTH = 800, HEIGHT = 600;
//const float toRad = 3.141592f / 180.0f; // Save the degree to radian conversion constant
GLuint VAO, VBO, IBO, pShader; // Unsigned integer

bool direction = true, sizeDirection = true, angleDirection = true; // True: translate right; False: translate left
float triOffset = 0.0f, triOffsetMax = 0.7f, triIncrement = 0.005f; // Increment is related to local FPS if not limited
float size = 0.4f, sizeMax = 0.8f, sizeMin = 0.1f, sizeIncrement = 0.005f;
float angle = 0.0f, angleMax = 360.0, angleMin = 0.0f, angleIncrement = 0.1f;

// Vertex shader. Version 3.3.0 of GLSL (OpenGL Shading Language)
// Through the matrix model we can pass x, y and z movement all at once in a single variable
static const char* vShader = "                               \n\
#version 330                                                 \n\
                                                             \n\
layout(location=0) in vec3 pos;                              \n\
                                                             \n\
out vec4 vColor;                                             \n\
                                                             \n\
uniform mat4 model;                                          \n\
                                                             \n\
void main(){                                                 \n\
 gl_Position = model * vec4(pos, 1.0);                       \n\
 vColor = vec4(clamp(pos, 0.0f, 1.0f), 1.0f);                \n\
}                                                            \n";

// Fragment shader
// *uniform*, dynamically assigned parameters (connection)
static const char* fShader = "                 \n\
#version 330                                   \n\
                                               \n\
uniform vec3 triangleColor;                    \n\
                                               \n\
in vec4 vColor;                                \n\
                                               \n\
out vec4 color;                                \n\
                                               \n\
void main(){                                   \n\
 color = vColor;                               \n\
}                                              \n";

// Function for creating a triangle (VAO and VBO)  
void CreateTriangle() {
	GLfloat vertex[] = {
		 0.0f,  1.0f, 0.0f, // Vertex 0 (x, y, z)
		 1.0f, -1.0f, 0.0f, // Vertex 1 (x, y, z)
		-1.0f, -1.0f, 0.0f, // Vertex 2 (x, y, z)
		 0.0f,  0.0f, 1.0f  // Vertex 3 (x, y, z)
	};

	unsigned int indices[]{
		0, 1, 2, // Pyramid front
		0, 1, 3, // Pyramid right
		0, 2, 3, // Pyramid left
		1, 2, 3  // Pyramid base
	};

	// VAO (Vertex Array Object), stored in RAM. Coordinates VBO buffering.
	glGenVertexArrays(1, &VAO); // Generates a VAO ID
	glBindVertexArray(VAO); // Binds ID to VAO

		// Loads index data into GPU memory
		// IBO (Index Buffer Object), stored in GPU memory
		glGenBuffers(1, &IBO); // Generates an IBO ID
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO); // Binds ID to IBO. IBO is automatically linked to its VAO
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); // Assigning the index values to the IBO

			// Loads vertex data into GPU memory
			// VBO (Vertex Buffer Object), stored in GPU memory
			glGenBuffers(1, &VBO); // Generates a VBO ID
			glBindBuffer(GL_ARRAY_BUFFER, VBO); // Binds ID to VBO. VBO is automatically linked to its VAO
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW); // Assigning the vertex values to the VBO
					// GL_STATIC_DRAW: used for fixed vertex (allocation of slower GPU memory)
					// GL_DYNAMIC_DRAW: used for dynamic vertex (allocation of faster GPU memory)
					// GL_STREAM_DRAW: vertex shows up a single frame

				// Attribute Pointer
				/* Args: (shader location, number of vertex in the primitive, type, is it all in one line? (normalized), do I need to skip something?,
				offset from the start?) */
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(0); // Arg: (shader location)

			glBindBuffer(GL_ARRAY_BUFFER, 0); // Reset VBO pointer for the next object to be processed
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // Reset IBO pointer for the next object to be processed

	glBindVertexArray(0); // Reset VAO pointer for the next object to be processed
}

// Function to create shaders
void CreateShader(GLenum shaderType, const char* shaderCode) {
	GLuint shader = glCreateShader(shaderType);
	const GLchar* code[1]; // Converting (char *) to an array of (GLchar *)
	code[0] = shaderCode; // Position 0 in the code array receives the shader code string
	GLint codeLength[1]; // Array of GLint
	codeLength[0] = strlen(shaderCode);
	// Args: (created shader, size of the array of codes, array of codes, array of 'code lengths') 
	// Here we only have one code, but we could have multiple
	glShaderSource(shader, 1, code, codeLength);

	// Compile the shader
	glCompileShader(shader);
	// Check if the compilation went OK
	GLint returnCode = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &returnCode); // Returns the compilation status to our returnCode variable
	if (!returnCode) {
		GLchar log[1024] = { 0 }; // 1024 is the standard max log size. Set to empty string
		glGetProgramInfoLog(shader, sizeof(log), NULL, log); // Get error log
		printf("%d shader compile error: '%s'\n", shaderType, log);
		return;
	}

	// Attach the executable (shader) to the program (pShader)
	glAttachShader(pShader, shader);
}

// Function to create the program (pShader: GLuint ID) and compile & attach the shaders to it
void CompileShader() {
	pShader = glCreateProgram();
	if (!pShader) {
		printf("Error while creating shader program");
		return;
	}

	CreateShader(GL_VERTEX_SHADER, vShader); // Create Vertex Shader and attach it to the program
	CreateShader(GL_FRAGMENT_SHADER, fShader); // Create Fragment Shader and attach it to the program

	// Link the program
	glLinkProgram(pShader);
	// Check if the link went OK
	GLint returnCode = 0;
	glGetProgramiv(pShader, GL_LINK_STATUS, &returnCode); // Returns the linking status to our returnCode variable
	if (!returnCode) {
		GLchar log[1024] = { 0 }; // 1024 is the standard max log size. Set to empty string
		glGetProgramInfoLog(pShader, sizeof(log), NULL, log); // Get error log
		printf("Program linking error: '%s'\n", log);
		return;
	}

	// Program validation (last step in the pipeline)
	glValidateProgram(pShader);
	returnCode = 0;
	glGetProgramiv(pShader, GL_VALIDATE_STATUS, &returnCode); // Returns the validation status to our returnCode variable
	if (!returnCode) {
		GLchar log[1024] = { 0 }; // 1024 is the standard max log size. Set to empty string
		glGetProgramInfoLog(pShader, sizeof(log), NULL, log); // Get error log
		printf("Program validation error: '%s'\n", log);
		return;
	}
}

int main() {

	/********************************
	*	Guarantee Compatibility
	*********************************/
	// Initialize the GLFW API
	if (!glfwInit()) {
		printf("GLFW was not initialized");
		glfwTerminate(); // Remove all traces from memory
		return 1;
	}

	// MAX MIN CONTEXT VERSIONS. Indicate to GLFW the min and max supported OpenGL Versions
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // (3.0 - 3.9)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	/* CORE PROFILE.
	GLFW is backward compatible by default. If a function is not encountered it searches on previous versions.
	We want it to only use the most current available API. Thus we set it's Core Profile to match the MAX MIN
	set above */
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// FORWARD FUNCTIONS. Newer implementations should be used if available.
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);


	/********************************
	*	Create Window
	*********************************/
	GLFWwindow* mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "New Window", NULL, NULL);
	// Check if the window was created correctly
	if (!mainWindow) {
		printf("GLFW didn't create the window");
		glfwTerminate();
		return 1;
	}

	// Get the framebuffer size needed (in pixels) for the Window (height x width) and stores in GPU memory
	int bufferWidth, bufferHeight;
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	// Set/Activate the main window
	glfwMakeContextCurrent(mainWindow);

	// GLEW EXPERIMENTAL. Allows GLEW to use experimental OpenGL extensions
	glewExperimental = GL_TRUE;

	// From this point on GLEW will automatically check for the best implementations available
	// We initialize GLEW on this line because the preceding code does not implement OpenGL
	if (glewInit() != GLEW_OK) {
		printf("GLEW did not initialize");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	// VIEWPORT configuration, passing framebuffer size in pixels
	glViewport(0, 0, bufferWidth, bufferHeight);

	// Create the triangle and compile the shader
	CreateTriangle(); // Set the data in the GPU memory
	CompileShader();

	auto t_start = std::chrono::high_resolution_clock::now();

	// Run till window gets closed
	while (!glfwWindowShouldClose(mainWindow)) {
		// Activate inputs and events (mouse and keyboard input, for instance)
		glfwPollEvents();

		/********************************
		*	Background Color
		*********************************/
		// Clear window and select a new color
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		// Load the selected color in the GPU memory buffer
		glClear(GL_COLOR_BUFFER_BIT);

		/********************************
		*	Triangle
		*********************************/
		glUseProgram(pShader); // Use the program which we put in the GPU memory
		glBindVertexArray(VAO); // Binds ID to VAO

		auto t_now = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_start).count();

		// GPU memory works with *int*
		GLint uniColor = glGetUniformLocation(pShader, "triangleColor"); // Searches for the 'triangleColor' variable in the pShader program
		float r = (sin(time * 4.0f) + 1.0f) / 2.0f;
		float g = 0.0f;
		float b = 0.0f;
		glUniform3f(uniColor, r, g, b); // Assigns the color read above

		/********************************
		*	Translate, Scale and Rotate
		*********************************/
		// Movement rule
		if (direction)
			triOffset += triIncrement; // Translate right
		else
			triOffset -= triIncrement; // Translate left

		if (abs(triOffset) >= triOffsetMax) // If predetermined max offset was reached, change direction
			direction = !direction;

		// Scaling rule
		if (sizeDirection)
			size += sizeIncrement; // Scale up
		else
			size -= sizeIncrement; // Scale down

		if (size >= sizeMax || size <= sizeMin) // If predetermined max/min scaling was reached, change scaling direction
			sizeDirection = !sizeDirection;

		// Rotation rule
		if (angleDirection)
			angle += angleIncrement; // Rotate left
		else
			angle -= angleIncrement; // Rotate right

		if (angle >= angleMax || angle <= angleMin) // If predetermined max/min angle was reached, change rotation direction
			angleDirection = !angleDirection;

		GLint uniModel = glGetUniformLocation(pShader, "model"); // Searches for the 'model' variable in the pShader program
		glm::mat4 model(1.0f); // glm::mat4 get the 'mat4' function inside the class 'glm'.
							   // Fill the (4x4) model matrix with 1's

		// Movement: model gets updated by the translate function
		//model = glm::translate(model, glm::vec3(triOffset, 0.0f, 0.0f)); // glm::vec3 returns the specified vector in the correct format

		// Rotate: model gets updated by the rotation funtion
		model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f)); // glm::vec3 returns the specified vector in the correct format

		// Scale: model gets updated by the scaling function
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f)); // glm::vec3 returns the specified vector in the correct format

		// Args: (shader model, number of matrices, should be transposed?, offset model values)
		glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model)); // Assigns the new offset model to the shader uniModel

		// Old way: calculating offset per axis, without using the matrix model
		//glUniform1f(uniXTranslate, triOffset); // Assigns the new calculated offset

		/********************************
		*	Draw Pyramid
		*********************************/
		/* The binding below is used to guarantee that old GPUs with no default index support do receive the indices. 
		The index implementation is recent and only supported in the 20 and 30 series of NVIDIA GPUs */
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO); // Binds ID to IBO.
			// Args: (primitive, index count (points to be connected), index type, end)
			glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // Reset IBO pointer for the next object to be processed


		//glDrawArrays(GL_TRIANGLES, 0, 3); // Args: (primitive vertex, first location position, number of coordinates/vertex)
		glBindVertexArray(0); // Reset VAO pointer for the next object to be processed
		glUseProgram(0); // Reset program pointer for the next program to be executed

		/********************************
		*	Update Screen
		*********************************/
		// Swap buffer -> Executes the instructions queued in the memory buffer
		glfwSwapBuffers(mainWindow);
	}

	return 0;
}