#include "Texture.h"

Texture::Texture() {
	textureID = 0;
	width = 0;
	height = 0;
	bitDepth = 0;
	fileLocation = NULL;
}

Texture::Texture(char* fileLoc) {
	textureID = 0;
	width = 0;
	height = 0;
	bitDepth = 0;
	fileLocation = fileLoc;
}

void Texture::loadTexture() {
	// 'stbi_load' stores the width, height and bit depth of the loaded image in the addresses passed to it
	/* Args: (file location, address where w will be returned, address where h will be returned, address where the bitD
	will be returned, desired channel) */
	unsigned char* texData = stbi_load(fileLocation, &width, &height, &bitDepth, 0);
	if (!texData) {
		printf("Failed to load image: '%s'\n", fileLocation);
	}

	glGenTextures(1, &textureID); // Generates texture and returns an ID
	glBindTexture(GL_TEXTURE_2D, textureID); // Binds texture in memory

		// Image filters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // GL_REPEAT on the x axis
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // GL_REPEAT on the y axis
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Blurred filtering closeup
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Blurred filtering far away

		// Stores image in the GPU memory
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData); // PNG has transparency and can be used with GL_RGBA
		// MIPMAP - resizes image multiple times for increased performance when dealing with texturing in variable depths
		glGenerateMipmap(GL_TEXTURE_2D); // Generate mipmaps

	glBindTexture(GL_TEXTURE_2D, 0); // Reset texture pointer for the next texture to be processed
	stbi_image_free(texData); // Free RAM allocation for the loaded image
}

void Texture::useTexture() {
	// The following line "opens the communication"/"sets the value" of the 'sampler' uniform variable in the Fragment Shader 
	glActiveTexture(GL_TEXTURE0); // Activate texture 0
	glBindTexture(GL_TEXTURE_2D, textureID); // Binds texture in memory
}

void Texture::clearTexture() {
	glDeleteTextures(1, &textureID);
	textureID = 0;
	width = 0;
	height = 0;
	bitDepth = 0;
	fileLocation = NULL;
}

Texture::~Texture() {
	clearTexture();
}