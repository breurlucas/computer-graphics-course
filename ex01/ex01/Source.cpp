#include <stdio.h>
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <string.h>

// By using the OpenGL types (like GLint) GLEW ensures we are using the most optimized implementations (memory allocation, for instance)
const GLint WIDTH = 800, HEIGHT = 600;
GLuint VAO, VBO, pShader; // Unsigned integer

// Vertex shader. Version 3.3.0 of GLSL (OpenGL Shading Language)
static const char *vShader = "                 \n\
#version 330                                   \n\
                                               \n\
layout(location=0) in vec2 pos;                \n\
                                               \n\
void main(){                                   \n\
 gl_Position = vec4(pos.x, pos.y, 0.0, 1.0);   \n\
}                                              \n";

// Fragment shader
static const char *fShader = "                 \n\
#version 330                                   \n\
                                               \n\
out vec4 color;                                \n\
                                               \n\
void main(){                                   \n\
 color = vec4(1.0, 1.0, 0.0, 1.0);             \n\
}                                              \n";

// Function for creating a triangle (VAO and VBO)
void CreateTriangle() {
	GLfloat vertex[] = {   
		-1.0f, -1.0f, // Vertex 1 (x, y)
		 1.0f, -1.0f, // Vertex 2 (x, y)
		 0.0f,  1.0f, // Vertex 3 (x, y)
	};

	// VAO (Vertex Array Object), stored in RAM. Coordinates VBO buffering.
	glGenVertexArrays(1, &VAO); // Generates a VAO ID
	glBindVertexArray(VAO); // Binds ID to VAO
		// Loads vertex data into GPU memory
		// VBO (Vertex Buffer Object), stored in GPU memory
		glGenBuffers(1, &VBO); // Generates a VBO ID
		glBindBuffer(GL_ARRAY_BUFFER, VBO); // Binds ID to VBO. VBO is automatically linked to its VAO
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW); // Assigning the vertex values to the VBO
			// GL_STATIC_DRAW: used for fixed vertex (slower memory allocation)
			// GL_DYNAMIC_DRAW: used for dynamic vertex (faster memory allocation)
			// GL_STREAM_DRAW: vertex shows up a single frame
			
			// Attribute Pointer
			/* Args: (shader location, number of vertex in the primitive, type, is it all in one line? (normalized), do I need to skip something?, 
			offset from the start?) */
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(0); // Arg: (shader location)

		glBindBuffer(GL_ARRAY_BUFFER, 0); // Remove unnecessary VBO data from memory for the next object to be processed
	   
	glBindVertexArray(0); // Remove unnecessary VAO data from memory for the next object to be processed
}

// Function to create shaders
void CreateShader(GLenum shaderType, const char *shaderCode) {
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
		GLchar log[1024] = {0}; // 1024 is the standard max log size. Set to empty string
		glGetProgramInfoLog(shader, sizeof(log), NULL, log); // Get error log
		printf("%d shader compile error: '%s'\n", shaderType, log);
		return;
	}

	// Attach the executable (shader) to the program (pShader)
	glAttachShader(pShader, shader);
}

// Function to compile the shader program (pShader -> GLuint ID)
void CompileShader() {
	pShader = glCreateProgram();
	if (!pShader) { 
		printf("Error while creating shader program");
		return;
	}

	CreateShader(GL_VERTEX_SHADER, vShader); // Create Vertex Shader
	CreateShader(GL_FRAGMENT_SHADER, fShader); // Create Fragment Shader 

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

	// ******** Guarantee compatibility ********

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

	
	// ******** Create window ********

	GLFWwindow *mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "New Window", NULL, NULL);
	// Check if the window was created correctly
	if (!mainWindow) {
		printf("GLFW didn't create the window");
		glfwTerminate();
		return 1;
	}

	// Get the framebuffer size needed (in pixels) for the Window (height x width) and stores in memory
	int bufferWidth, bufferHeight;
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	// Set the main window
	glfwMakeContextCurrent(mainWindow);

	// GLEW EXPERIMENTAL. Allows GLEW to use experimental OpenGL extensions
	glewExperimental = GL_TRUE;

	// From this point on GLEW will automatically check for the best implementations available
	if (glewInit() != GLEW_OK) {
		printf("GLEW did not initialize");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	// VIEWPORT configuration, passing framebuffer size in pixels
	glViewport(0, 0, bufferWidth, bufferHeight); 

	// Create the triangle and compile the shader
	CreateTriangle();
	CompileShader();

	// Run till window gets closed
	while (!glfwWindowShouldClose(mainWindow)) {
		// Activate inputs and events (mouse and keyboard input, for instance)
		glfwPollEvents();
		// Clear window and select a new color
		glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
		// Load the selected color in the GPU memory buffer
		glClear(GL_COLOR_BUFFER_BIT);

		// Draw the created triangle
		glUseProgram(pShader);
			glBindVertexArray(VAO);
				// Args: (primitive vertex, location, number of coordinates)
				glDrawArrays(GL_TRIANGLES, 0, 3);
			glBindVertexArray(0);
		glUseProgram(0);

		// Swap buffer -> Executes the instructions queued in the memory buffer
		glfwSwapBuffers(mainWindow);
	}

	return 0;
}