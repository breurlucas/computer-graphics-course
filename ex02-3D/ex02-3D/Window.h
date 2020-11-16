#pragma once
#include <stdio.h>
#include <GL\glew.h>
#include <GLFW\glfw3.h>

class Window
{
public:
	Window();
	Window(GLint windowWidth, GLint windowHeight);
	~Window();
	int Initialize();
	void swapBuffer();

	GLfloat getBufferWidth() { return (GLfloat)bufferWidth; };
	GLfloat getBufferHeight() { return (GLfloat)bufferHeight; };
	bool getWindowShouldClose() { return glfwWindowShouldClose(mainWindow); };
	bool* getKeys() { return keys; };
	GLfloat getXChange();
	GLfloat getYChange();

private:
	GLFWwindow* mainWindow;
	// By using the OpenGL types (like GLint) GLEW ensures we are using the most optimized implementations (memory allocation, for instance)
	GLint width, height;
	GLint bufferWidth, bufferHeight;

	// Keyboard/mouse callback config
	void createCallbacks();

	// Keyboard config
	bool keys[1024]; // UTF-8
	static void handleKeys(GLFWwindow* window, int key, int code, int action, int mode);
	
	// Mouse config
	bool mouseFirstMove; // Check if it is the first move (React based off offset)
	GLfloat xLast, yLast, xChange, yChange;
	static void handleMouse(GLFWwindow* window, double xPos, double yPos);
};

