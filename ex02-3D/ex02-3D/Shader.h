#pragma once
#include <stdio.h>
#include <string>
#include <GL\glew.h>

#include <iostream>
#include <fstream>

#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"

class Shader
{
public:
	Shader();
	~Shader();
	void CreateFromString(const char* vertexCode, const char* fragmentCode);
	void CreateFromFile(const char* vertexLocation, const char* fragmentLocation);
	void UseProgram();

	void setDirectionalLight(DirectionalLight* dLight);
	void setPointLight(PointLight* pLight, unsigned int lightsCount);
	void setSpotLight(SpotLight* sLight, unsigned int lightsCount);

	// Getters
	GLuint getUniformProjection() { return uniformProjection; };
	GLuint getUniformModel() { return uniformModel; };
	GLuint getUniformView() { return uniformView; };
	GLuint getUniformEyePosition() { return uniformEyePosition; };
	GLuint getUniformSpecularIntensity() { return uniformSpecularIntensity; };
	GLuint getUniformShininess() { return uniformShininess; };

private:
	GLuint shaderID, uniformProjection, uniformModel, uniformView, uniformEyePosition,
		uniformSpecularIntensity, uniformShininess;

	struct
	{
		GLuint uniformAmbientIntensity;
		GLuint uniformAmbientColor;
		GLuint uniformDiffuseIntensity;
		GLuint uniformDirection;
	} uniformDirectionalLight;

	GLuint uniformPointLightsCount;
	struct {
		GLuint uniformAmbientIntensity;
		GLuint uniformAmbientColor;
		GLuint uniformDiffuseIntensity;
		GLuint uniformPosition;
		GLuint uniformConstant;
		GLuint uniformLinear;
		GLuint uniformExponent;
	} uniformPointLights[MAX_POINT_LIGHTS]; // We can have more than one point light in a scene

	GLuint uniformSpotLightsCount;
	struct {
		GLuint uniformAmbientIntensity;
		GLuint uniformAmbientColor;
		GLuint uniformDiffuseIntensity;
		GLuint uniformPosition;
		GLuint uniformDirection;
		GLuint uniformConstant;
		GLuint uniformLinear;
		GLuint uniformExponent;
		GLuint uniformEdge;
	} uniformSpotLights[MAX_SPOT_LIGHTS];

	void CreateShader(const char *vertexCode, const char *fragmentCode);
	void CompileShader(GLenum shaderType, const char *shaderCode);
	std::string ReadFile(const char* fileLocation);
};

