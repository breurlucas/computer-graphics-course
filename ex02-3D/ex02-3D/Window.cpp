#include "Window.h"

Window::Window() {
	width = 800;
	height = 600;

	// Initialize keyboard keys array
	for (int i=0; i < 1024; i++) {
		keys[i] = false;
	}

	xChange = 0.0f;
	yChange = 0.0f;
	mouseFirstMove = true;
}

Window::Window(GLint windowWidth, GLint windowHeight) {
	width = windowHeight;
	height = windowHeight;

	// Initialize keyboard keys array
	for (int i = 0; i < 1024; i++) {
		keys[i] = false;
	}

	xChange = 0.0f;
	yChange = 0.0f;
	mouseFirstMove = true;
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

	// Set up callbacks for keyboard and mouse inputs
	createCallbacks();

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
	/*
	Sets pointer to the 'mainWindow' object in order for it to be accessible by the static methods for keyboard and
	mouse handling; such as: 'handleKeys()'

	The pointer is 'this', the current class
	*/
	glfwSetWindowUserPointer(mainWindow, this); 
}

void Window::swapBuffer() {
	glfwSwapBuffers(mainWindow);
}

// Args are automatically filled in by the 'glfwSetKeyCallback()' function
// Args: (window, key, code related to the key, action (press, release etc.), mode related to the action)
void Window::handleKeys(GLFWwindow* window, int key, int code, int action, int mode) {
	/*
	Here we return the pointer to 'mainWindow' stored previously. Thus we are returning the class 'Window'.
	The type returned is generic so a casting to <Window*> is necessary.
	*/
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	// Close the window when 'esc' is pressed
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
		return;
	}

	// Store key presses/releases in the keys array
	if (key >= 0 && key <= 1024) { 	// Guarantees conformity to our UTF-8 array size
		if (action == GLFW_PRESS) {
			theWindow->keys[key] = true;
			//printf("Pressed %d\n", key);
		}
		else if (action == GLFW_RELEASE) {
			theWindow->keys[key] = false;
			//printf("Released %d\n", key);
		}
	}
}

void Window::handleMouse(GLFWwindow* window, double xPos, double yPos) {
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (theWindow->mouseFirstMove) {
		theWindow->xLast = xPos;
		theWindow->yLast = yPos;
		theWindow->mouseFirstMove = false;
	}

	theWindow->xChange = xPos - theWindow->xLast;
	theWindow->yChange = theWindow->yLast - yPos;

	theWindow->xLast = xPos;
	theWindow->yLast = yPos;

	//printf("x: %.2f y: %.2f\n", theWindow->xChange, theWindow->yChange);
}


void Window::createCallbacks() {
	glfwSetKeyCallback(mainWindow, handleKeys);
	glfwSetCursorPosCallback(mainWindow, handleMouse);
}

GLfloat Window::getXChange() {
	GLfloat theChange = xChange;
	xChange = 0; // Reset after read
	return theChange;
}

GLfloat Window::getYChange() {
	GLfloat theChange = yChange;
	yChange = 0; // Reset after read
	return theChange;
}


