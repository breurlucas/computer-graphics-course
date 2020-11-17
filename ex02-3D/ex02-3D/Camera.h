#pragma once
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>


class Camera
{
public:
	Camera();
	Camera(glm::vec3 startPosition, glm::vec3 startWorldUp, GLfloat startYaw, GLfloat startPitch, GLfloat startMoveSpeed, GLfloat startTurnSpeed);
	~Camera();

	void keyControl(bool* keys, GLfloat deltaTime);
	void mouseControl(GLfloat xChange, GLfloat yChange, GLfloat deltaTime);

	glm::mat4 calculateViewMatrix();

	glm::vec3 getCameraPosition() { return position; };

private:
	void update();

	// Related to keyboard movement
	glm::vec3 position; // Camera position
	glm::vec3 front; // Z
	glm::vec3 up; // Y
	glm::vec3 right; // X

	// Related to any type of movement
	glm::vec3 worldUp; // Sky/Terrain position

	// Related to mouse movement
	GLfloat yaw; // Right/Left
	GLfloat pitch; // Up/Down

	// First is related to keyboard, second to mouse
	GLfloat moveSpeed, turnSpeed;
};

