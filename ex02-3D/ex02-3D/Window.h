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

private:
	GLFWwindow* mainWindow;
	// By using the OpenGL types (like GLint) GLEW ensures we are using the most optimized implementations (memory allocation, for instance)
	GLint width, height;
	GLint bufferWidth, bufferHeight;
};

