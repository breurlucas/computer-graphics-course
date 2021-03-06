#include "Shader.h"

Shader::Shader() {
	shaderID = 0;
	uniformModel = 0;
	uniformProjection = 0;
	//uniformView = 0;
}

Shader::~Shader() {
	if (shaderID != 0) {
		glDeleteProgram(shaderID);
		shaderID = 0;
	}
	uniformModel = 0;
	uniformProjection = 0;
	//uniformView = 0;
}

void Shader::UseProgram() {
	glUseProgram(shaderID);
}

// Public access to the compiling method
void Shader::CreateFromString(const char* vertexCode, const char* fragmentCode) {
	CreateShader(vertexCode, fragmentCode);
}

void Shader::CreateFromFile(const char* vertexLocation, const char* fragmentLocation) {
	std::string vertexString = ReadFile(vertexLocation);
	std::string fragmentString = ReadFile(fragmentLocation);

	const char* vertexCode = vertexString.c_str(); // String to char* convertion
	const char* fragmentCode = fragmentString.c_str();

	CreateShader(vertexCode, fragmentCode);
}

std::string Shader::ReadFile(const char* fileLocation) {
	std::string content = "";
	std::ifstream fileStream(fileLocation, std::ios::in);

	if (!fileStream.is_open()) {
		printf("Error while trying to open the following file: '%s'\n", fileLocation);
		return "";
	}

	std::string line;
	while (!fileStream.eof()) {
		std::getline(fileStream, line);
		content.append(line + '\n');
	}

	fileStream.close();
	return content;
}

void Shader::CreateShader(const char* vertexCode, const char* fragmentCode) {
	shaderID = glCreateProgram(); // Program ID in the GPU
	if (!shaderID) {
		printf("Error while creating shader program\n");
		return;
	}

	CompileShader(GL_VERTEX_SHADER, vertexCode); // Create Vertex Shader and attach it to the program
	CompileShader(GL_FRAGMENT_SHADER, fragmentCode); // Create Fragment Shader and attach it to the program

	// Link the program
	glLinkProgram(shaderID);
	// Check if the link went OK
	GLint returnCode = 0;
	glGetProgramiv(shaderID, GL_LINK_STATUS, &returnCode); // Returns the linking status to our returnCode variable
	if (!returnCode) {
		GLchar log[1024] = { 0 }; // 1024 is the standard max log size. Set to empty string
		glGetProgramInfoLog(shaderID, sizeof(log), NULL, log); // Get error log
		printf("Program linking error: '%s'\n", log);
		return;
	}

	// Program validation (last step in the pipeline)
	glValidateProgram(shaderID);
	returnCode = 0;
	glGetProgramiv(shaderID, GL_VALIDATE_STATUS, &returnCode); // Returns the validation status to our returnCode variable
	if (!returnCode) {
		GLchar log[1024] = { 0 }; // 1024 is the standard max log size. Set to empty string
		glGetProgramInfoLog(shaderID, sizeof(log), NULL, log); // Get error log
		printf("Program validation error: '%s'\n", log);
		return;
	}

	// Get the uniform variables in the compiled shaders and store them in memory
	uniformProjection = glGetUniformLocation(shaderID, "projection"); // Searches for the 'projection' variable in the shader program
	uniformModel = glGetUniformLocation(shaderID, "model"); // Searches for the 'model' variable in the shader program
	uniformView = glGetUniformLocation(shaderID, "view"); // Searches for the 'view' variable in the shader program
	// Ambient Light
	uniformDirectionalLight.uniformAmbientColor = glGetUniformLocation(shaderID, "directionalLight.base.color");
	uniformDirectionalLight.uniformAmbientIntensity = glGetUniformLocation(shaderID, "directionalLight.base.ambientIntensity");
	// Diffuse Light
	uniformDirectionalLight.uniformDirection = glGetUniformLocation(shaderID, "directionalLight.direction");
	uniformDirectionalLight.uniformDiffuseIntensity = glGetUniformLocation(shaderID, "directionalLight.base.diffuseIntensity");
	// Specular Light
	uniformSpecularIntensity = glGetUniformLocation(shaderID, "material.specularIntensity");
	uniformShininess = glGetUniformLocation(shaderID, "material.shininess");
	// Point Light
	uniformPointLightsCount = glGetUniformLocation(shaderID, "pointLightsCount");
	for (int i = 0; i < MAX_POINT_LIGHTS; i++) {
		char locBuff[100] = { "\0" };
		snprintf(locBuff, sizeof(locBuff), "pointLights[%d].base.ambientIntensity", i);
		uniformPointLights[i].uniformAmbientIntensity = glGetUniformLocation(shaderID, locBuff);

		snprintf(locBuff, sizeof(locBuff), "pointLights[%d].base.color", i);
		uniformPointLights[i].uniformAmbientColor = glGetUniformLocation(shaderID, locBuff);

		snprintf(locBuff, sizeof(locBuff), "pointLights[%d].base.diffuseIntensity", i);
		uniformPointLights[i].uniformDiffuseIntensity = glGetUniformLocation(shaderID, locBuff);

		snprintf(locBuff, sizeof(locBuff), "pointLights[%d].position", i);
		uniformPointLights[i].uniformPosition = glGetUniformLocation(shaderID, locBuff);

		snprintf(locBuff, sizeof(locBuff), "pointLights[%d].constant", i);
		uniformPointLights[i].uniformConstant = glGetUniformLocation(shaderID, locBuff);

		snprintf(locBuff, sizeof(locBuff), "pointLights[%d].linear", i);
		uniformPointLights[i].uniformLinear = glGetUniformLocation(shaderID, locBuff);

		snprintf(locBuff, sizeof(locBuff), "pointLights[%d].exponent", i);
		uniformPointLights[i].uniformExponent = glGetUniformLocation(shaderID, locBuff);
	}
	// Spot Light
	uniformSpotLightsCount = glGetUniformLocation(shaderID, "spotLightsCount");
	for (int i = 0; i < MAX_SPOT_LIGHTS; i++) {
		char locBuff[100] = { "\0" };
		snprintf(locBuff, sizeof(locBuff), "spotLights[%d].point.base.ambientIntensity", i);
		uniformSpotLights[i].uniformAmbientIntensity = glGetUniformLocation(shaderID, locBuff);

		snprintf(locBuff, sizeof(locBuff), "spotLights[%d].point.base.color", i);
		uniformSpotLights[i].uniformAmbientColor = glGetUniformLocation(shaderID, locBuff);

		snprintf(locBuff, sizeof(locBuff), "spotLights[%d].base.diffuseIntensity", i);
		uniformSpotLights[i].uniformDiffuseIntensity = glGetUniformLocation(shaderID, locBuff);

		snprintf(locBuff, sizeof(locBuff), "spotLights[%d].point.position", i);
		uniformSpotLights[i].uniformPosition = glGetUniformLocation(shaderID, locBuff);

		snprintf(locBuff, sizeof(locBuff), "spotLights[%d].direction", i);
		uniformSpotLights[i].uniformDirection = glGetUniformLocation(shaderID, locBuff);

		snprintf(locBuff, sizeof(locBuff), "spotLights[%d].point.constant", i);
		uniformSpotLights[i].uniformConstant = glGetUniformLocation(shaderID, locBuff);

		snprintf(locBuff, sizeof(locBuff), "spotLights[%d].point.linear", i);
		uniformSpotLights[i].uniformLinear = glGetUniformLocation(shaderID, locBuff);

		snprintf(locBuff, sizeof(locBuff), "spotLights[%d].point.exponent", i);
		uniformSpotLights[i].uniformExponent = glGetUniformLocation(shaderID, locBuff);

		snprintf(locBuff, sizeof(locBuff), "spotLights[%d].edge", i);
		uniformSpotLights[i].uniformEdge = glGetUniformLocation(shaderID, locBuff);
	}
}

void Shader::CompileShader(GLenum shaderType, const char* shaderCode) {
	GLuint shader = glCreateShader(shaderType);
	const GLchar* code[1]; // Converting (char *) to an array of (GLchar *)
	code[0] = shaderCode; // Position 0 in the code array receives the shader code string

	// Args: (created shader, size of the array of codes, array of codes, array of 'code lengths') 
	// Here we only have one code, but we could have multiple
	glShaderSource(shader, 1, code, NULL);

	// Compile the shader
	glCompileShader(shader);
	// Check if the compilation went OK
	GLint returnCode = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &returnCode); // Returns the compilation status to our returnCode variable
	if (!returnCode) {
		GLchar log[1024] = { 0 }; // 1024 is the standard max log size. Set to empty string
		glGetShaderInfoLog(shader, sizeof(log), NULL, log); // Get error log
		printf("%d shader compile error: '%s'\n", shaderType, log);
		return;
	}

	// Attach the executable (shader) to the program (pShader)
	glAttachShader(shaderID, shader);
}

void Shader::setDirectionalLight(DirectionalLight* dLight) {
	dLight->useLight(uniformDirectionalLight.uniformAmbientIntensity, uniformDirectionalLight.uniformAmbientColor,
					uniformDirectionalLight.uniformDiffuseIntensity, uniformDirectionalLight.uniformDirection);
}

void Shader::setPointLight(PointLight* pLight, unsigned int lightsCount) {
	if (lightsCount > MAX_POINT_LIGHTS) lightsCount = MAX_POINT_LIGHTS;
	glUniform1i(uniformPointLightsCount, lightsCount);
	for (int i=0; i < lightsCount; i++) {
		pLight[i].UseLight(uniformPointLights[i].uniformAmbientIntensity, uniformPointLights[i].uniformAmbientColor,
						uniformPointLights[i].uniformDiffuseIntensity, uniformPointLights[i].uniformPosition,
						uniformPointLights[i].uniformConstant, uniformPointLights[i].uniformLinear, uniformPointLights[i].uniformExponent);
	}
}

void Shader::setSpotLight(SpotLight* sLight, unsigned int lightsCount) {
	if (lightsCount > MAX_SPOT_LIGHTS) lightsCount = MAX_SPOT_LIGHTS;
	glUniform1i(uniformSpotLightsCount, lightsCount);
	for (int i = 0; i < lightsCount; i++) {
		sLight[i].UseLight(uniformSpotLights[i].uniformAmbientIntensity, uniformSpotLights[i].uniformAmbientColor,
			uniformSpotLights[i].uniformDiffuseIntensity, uniformSpotLights[i].uniformPosition, uniformSpotLights[i].uniformDirection,
			uniformSpotLights[i].uniformConstant, uniformSpotLights[i].uniformLinear, uniformSpotLights[i].uniformExponent, uniformSpotLights[i].uniformEdge);
	}
}
