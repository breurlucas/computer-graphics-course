#include <stdio.h>
#include <GL\glew.h>
#include <GLFW\glfw3.h>

// By using the OpenGL types (like GLint) GLEW ensures we are using the most optimized implementations (memory allocation, for instance)
const GLint WIDTH = 800, HEIGHT = 600;

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


	// Run till window gets closed
	while (!glfwWindowShouldClose(mainWindow)) {
		// Activate inputs and events (mouse and keyboard input, for instance)
		glfwPollEvents();
		// Clear window and select a new color
		glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
		// Load the selected color in the GPU memory buffer
		glClear(GL_COLOR_BUFFER_BIT);
		// Swap buffer -> Executes the instructions queued in the memory buffer
		glfwSwapBuffers(mainWindow);
	}

	return 0;
}