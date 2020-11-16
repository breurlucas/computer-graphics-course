#pragma once
#include <GL\glew.h>

class Mesh
{
public:
	Mesh(); // Constructor
	~Mesh(); // Destructor. There is no garbage collector, we need to specify memory freeing
	// We are passing addresses (pointers) to arrays, thus, we need to indicate the array lengths as well
	void CreateMesh(GLfloat *vertices, unsigned int *indices, unsigned int numOfVertices, unsigned int numOfIndices); 
	void RenderMesh();

private:
	GLuint VAO, VBO, IBO;
	GLsizei indexCount;
};

