#include "Light.h"

Light::Light() {
	// Ambient
	color = glm::vec3(1.0f, 1.0f, 1.0f);
	ambientIntensity = 1.0f;
	// Diffuse
	direction = glm::vec3(0.0f, 0.0f, 0.0f);
	diffuseIntensity = 0.0f;
}

Light::Light(GLfloat red, GLfloat green, GLfloat blue, GLfloat aIntensity,
		GLfloat xDir, GLfloat yDir, GLfloat zDir, GLfloat dIntensity) {
	// Ambient
	color = glm::vec3(red, green, blue);
	ambientIntensity = aIntensity;
	// Diffuse
	direction = glm::vec3(xDir, yDir, zDir);
	diffuseIntensity = dIntensity;
}

void Light::useLight(GLuint ambientIntensityLocation, GLuint ambientColorLocation, 
				GLuint diffuseIntensityLocation, GLuint directionLocation) {
	// Ambient/General Lighting
	glUniform1f(ambientIntensityLocation, ambientIntensity);
	glUniform3f(ambientColorLocation, color.x, color.y, color.z);
	// Diffuse
	glUniform1f(diffuseIntensityLocation, diffuseIntensity);
	glUniform3f(directionLocation, direction.x, direction.y, direction.z);
}

Light::~Light() {}