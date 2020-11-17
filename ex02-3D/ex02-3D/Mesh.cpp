#include "Mesh.h"

Mesh::Mesh() {
	VAO = 0;
	VBO = 0;
	IBO = 0;
	indexCount = 0;
}

void Mesh::CreateMesh(GLfloat* vertices, unsigned int* indices, unsigned int numOfVertices, unsigned int numOfIndices) {
	indexCount = numOfIndices; // This way the 'numOfIndices' gets stored in the class attribute 'indexCount' and can be accessed by other methods

	// VAO (Vertex Array Object), stored in RAM. Coordinates VBO buffering.
	glGenVertexArrays(1, &VAO); // Generates a VAO ID
	glBindVertexArray(VAO); // Binds ID to VAO

		// Loads index data into GPU memory
		// IBO (Index Buffer Object), stored in GPU memory
		glGenBuffers(1, &IBO); // Generates an IBO ID
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO); // Binds ID to IBO. IBO is automatically linked to its VAO
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * numOfIndices, indices, GL_STATIC_DRAW); // Assigning the index values to the IBO

			// Loads vertex data into GPU memory
			// VBO (Vertex Buffer Object), stored in GPU memory
			glGenBuffers(1, &VBO); // Generates a VBO ID
			glBindBuffer(GL_ARRAY_BUFFER, VBO); // Binds ID to VBO. VBO is automatically linked to its VAO
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * numOfVertices, vertices, GL_STATIC_DRAW); // Assigning the vertex values to the VBO
					// GL_STATIC_DRAW: used for fixed vertex (allocation of slower GPU memory)
					// GL_DYNAMIC_DRAW: used for dynamic vertex (allocation of faster GPU memory)
					// GL_STREAM_DRAW: vertex shows up a single frame

				// Attribute Pointer
				/* Args: (shader location, number of vertex in the primitive, type, is it all in one line? (normalized), do I need to skip something?,
				offset from the start?) */
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * 5, 0); // 'sizeof(vertices[0]) * 5' skips to the next line after reading 3 elements
				// The shader location is now set to 1 instead of 0
				glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * 5, (void*) (sizeof(vertices[0]) * 3)); // '(void*) (sizeof(vertices[0]) * 3))' skips 3 elements when reading a line
				glEnableVertexAttribArray(0); // Arg: (shader location)
				glEnableVertexAttribArray(1); // Arg: (shader location)

			glBindBuffer(GL_ARRAY_BUFFER, 0); // Reset VBO pointer for the next object to be processed
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // Reset IBO pointer for the next object to be processed
	glBindVertexArray(0); // Reset VAO pointer for the next object to be processed
}

void Mesh::RenderMesh() {
	glBindVertexArray(VAO); // Binds ID to VAO
		/* The binding below is used to guarantee that old GPUs with no default index support do receive the indices.
		The index implementation is recent and only supported in the 20 and 30 series of NVIDIA GPUs for instance */
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO); // Binds ID to IBO.
			// Args: (primitive, index count (points to be connected), index type, end)
			glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // Reset IBO pointer for the next object to be processed
	glBindVertexArray(0); // Reset VAO pointer for the next object to be processed
}

Mesh::~Mesh() {
	if (VAO != 0) {
		glDeleteBuffers(1, &VAO);
	}
	if (VBO != 0) {
		glDeleteBuffers(1, &VBO);
	}
	if (IBO != 0) {
		glDeleteBuffers(1, &IBO);
	}
	indexCount = 0;
}
