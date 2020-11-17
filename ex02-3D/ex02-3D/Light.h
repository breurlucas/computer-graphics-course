#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>

class Light
{
public:
	Light();
	Light(GLfloat red, GLfloat green, GLfloat blue, GLfloat aIntensity,
		GLfloat xDir, GLfloat yDir, GLfloat zDir, GLfloat dIntensity);
	void useLight(GLuint ambientIntensityLocation, GLuint ambientColorLocation,
		GLuint diffuseIntensityLocation, GLuint directionLocation);
	~Light();

private:
	// Ambient
	glm::vec3 color;
	GLfloat ambientIntensity;
	// Diffuse
	glm::vec3 direction;
	GLfloat diffuseIntensity;
};

