#pragma once
#include <stdio.h>
#include <string>
#include <GL\glew.h>

#include <iostream>
#include <fstream>

class Shader
{
public:
	Shader();
	~Shader();
	void CreateFromString(const char* vertexCode, const char* fragmentCode);
	void CreateFromFile(const char* vertexLocation, const char* fragmentLocation);
	void UseProgram();

	// Getters
	GLuint getUniformProjection() { return uniformProjection; };
	GLuint getUniformModel() { return uniformModel; };
	GLuint getUniformView() { return uniformView; };
	GLuint getUniformEyePosition() { return uniformEyePosition; };
	GLuint getUniformAmbientIntensity() { return uniformAmbientIntensity; };
	GLuint getUniformAmbientColor() { return uniformAmbientColor; };
	GLuint getUniformDiffuseIntensity() { return uniformDiffuseIntensity; };
	GLuint getUniformDirection() { return uniformDirection; };
	GLuint getUniformSpecularIntensity() { return uniformSpecularIntensity; };
	GLuint getUniformShininess() { return uniformShininess; };

private:
	GLuint shaderID, uniformProjection, uniformModel, uniformView, uniformEyePosition,
		uniformAmbientIntensity, uniformAmbientColor,
		uniformDiffuseIntensity, uniformDirection,
		uniformSpecularIntensity, uniformShininess;
	void CreateShader(const char *vertexCode, const char *fragmentCode);
	void CompileShader(GLenum shaderType, const char *shaderCode);
	std::string ReadFile(const char* fileLocation);
};

