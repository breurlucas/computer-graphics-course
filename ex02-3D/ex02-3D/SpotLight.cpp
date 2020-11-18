#include "SpotLight.h"

SpotLight::SpotLight() : PointLight() {
	direction = glm::vec3(0.0f, -1.0f, 0.0f); // Setting y to -1 to prevent division by 0
	edge = 0.0f;
	edgeProc = cosf(glm::radians(edge));
}

SpotLight::SpotLight(GLfloat red, GLfloat green, GLfloat blue,
					GLfloat aIntensity, GLfloat dIntensity,
					GLfloat xPos, GLfloat yPos, GLfloat zPos, GLfloat xDir, GLfloat yDir, GLfloat zDir,
					GLfloat con, GLfloat lin, GLfloat exp, GLfloat edg) : 
					PointLight(red, green, blue, aIntensity, dIntensity, xPos, yPos, zPos, con, lin, exp) {
	direction = glm::vec3(xDir, yDir, zDir);
	edge = edg;
	edgeProc = cosf(glm::radians(edge));
}

SpotLight::~SpotLight() {}

void SpotLight::UseLight(GLuint ambientIntensityLocation, GLuint ambientColorLocation,
						GLuint diffuseIntensityLocation, GLuint positionLocation, GLuint directionalLocation,
						GLuint constantLocation, GLuint linearLocation, GLuint exponentLocation, GLuint edgeLocation) {
		// Ambient
		glUniform1f(ambientIntensityLocation, ambientIntensity);
		glUniform3f(ambientColorLocation, color.x, color.y, color.z);
		// Diffuse
		glUniform1f(diffuseIntensityLocation, diffuseIntensity);
		glUniform3f(positionLocation, position.x, position.y, position.z);

		glUniform3f(directionalLocation, direction.x, direction.y, direction.z);

		glUniform1f(constantLocation, constant);
		glUniform1f(linearLocation, linear);
		glUniform1f(exponentLocation, exponent);

		glUniform1f(edgeLocation, edgeProc);
}

void SpotLight::SetFlash(glm::vec3 pos, glm::vec3 dir) {
	position = pos;
	direction = dir;
}
