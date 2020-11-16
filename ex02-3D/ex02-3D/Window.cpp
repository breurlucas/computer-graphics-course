#include "Window.h"

Window::Window() {
	width = 800;
	height = 600;
}

Window::Window(GLint windowWidth, GLint windowHeight) {
	width = windowHeight;
	height = windowHeight;
}

Window::~Window() {
	glfwDestroyWindow(mainWindow);
	glfwTerminate();
}


int Window::Initialize() {
	/********************************
	*	Guarantee Compatibility
	*********************************/
	// Initialize the GLFW API
	if (!glfwInit()) {
		printf("GLFW was not initialized");
		glfwTerminate(); // Remove all traces from memory
		return 1;
	}
	// Max & Min CONTEXT VERSIONS 
	// Indicate to GLFW the min and max supported OpenGL Versions
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // (ex: 3.0 - 3.9)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	/* CORE PROFILE
	GLFW is backward compatible by default. If a function is not encountered it searches on previous versions.
	We want it to only use the most current available API. Thus we set it's Core Profile to match the MAX MIN
	set above */
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// FORWARD FUNCTIONS
	// Newer implementations should be used if available.
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	/********************************
	*	Create Window
	*********************************/
	mainWindow = glfwCreateWindow(width, height, "New Window", NULL, NULL);
	// Check if the window was created correctly
	if (!mainWindow) {
		printf("GLFW didn't create the window");
		glfwTerminate();
		return 1;
	}

	// Get the framebuffer size needed (in pixels) for the Window (height x width) and stores in GPU memory
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

	// Enable depth testing (fixes unwanted transparency)
	glEnable(GL_DEPTH_TEST);

	// Viewport configuration, passing framebuffer size in pixels
	glViewport(0, 0, bufferWidth, bufferHeight);
}

void Window::swapBuffer() {
	glfwSwapBuffers(mainWindow);
}
