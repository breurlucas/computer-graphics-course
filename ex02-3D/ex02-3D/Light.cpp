#include "Light.h"

Light::Light() {
	// Ambient
	color = glm::vec3(1.0f, 1.0f, 1.0f);
	ambientIntensity = 1.0f;
	// Diffuse
	diffuseIntensity = 0.0f;
}

Light::Light(GLfloat red, GLfloat green, GLfloat blue, GLfloat aIntensity, GLfloat dIntensity) {
	// Ambient
	color = glm::vec3(red, green, blue);
	ambientIntensity = aIntensity;
	// Diffuse
	diffuseIntensity = dIntensity;
}

Light::~Light() {}