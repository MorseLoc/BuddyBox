#include "Renderer.h"

#include <glad/glad.h>

// Creates the Renderer.
Renderer::Renderer()
{
	// 1.6
	// These vertices create a cube.
	//
	// A cube has 6 faces.
	// Each face needs 2 triangles.
	// Each triangle needs 3 vertices.
	//
	// 6 faces × 2 triangles × 3 vertices = 36 vertices.

	float cubeVertices[] =
	{
		// FRONT FACE
		-0.5f, -0.5f,  0.5f,    0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,    1.0f, 0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,    1.0f, 1.0f, 1.0f,

		 0.5f,  0.5f,  0.5f,    1.0f, 1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,    0.0f, 1.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,    0.0f, 0.0f, 1.0f,

		// BACK FACE
		-0.5f, -0.5f, -0.5f,    1.0f, 0.0f, 2.0f,
		-0.5f,  0.5f, -0.5f,    1.0f, 1.0f, 2.0f,
		 0.5f,  0.5f, -0.5f,    0.0f, 1.0f, 2.0f,

		 0.5f,  0.5f, -0.5f,    0.0f, 1.0f, 2.0f,
		 0.5f, -0.5f, -0.5f,    0.0f, 0.0f, 2.0f,
		-0.5f, -0.5f, -0.5f,    1.0f, 0.0f, 2.0f,

		// LEFT FACE
		-0.5f,  0.5f,  0.5f,    1.0f, 1.0f, 3.0f,
		-0.5f,  0.5f, -0.5f,    0.0f, 1.0f, 3.0f,
		-0.5f, -0.5f, -0.5f,    0.0f, 0.0f, 3.0f,

		-0.5f, -0.5f, -0.5f,    0.0f, 0.0f, 3.0f,
		-0.5f, -0.5f,  0.5f,    1.0f, 0.0f, 3.0f,
		-0.5f,  0.5f,  0.5f,    1.0f, 1.0f, 3.0f,

		// RIGHT FACE
		 0.5f,  0.5f,  0.5f,    0.0f, 1.0f, 4.0f,
		 0.5f, -0.5f,  0.5f,    0.0f, 0.0f, 4.0f,
		 0.5f, -0.5f, -0.5f,    1.0f, 0.0f, 4.0f,

		 0.5f, -0.5f, -0.5f,    1.0f, 0.0f, 4.0f,
		 0.5f,  0.5f, -0.5f,    1.0f, 1.0f, 4.0f,
		 0.5f,  0.5f,  0.5f,    0.0f, 1.0f, 4.0f,

		 // TOP FACE
		 -0.5f,  0.5f, -0.5f,    0.0f, 1.0f, 0.0f,
		 -0.5f,  0.5f,  0.5f,    0.0f, 0.0f, 0.0f,
		  0.5f,  0.5f,  0.5f,    1.0f, 0.0f, 0.0f,

		  0.5f,  0.5f,  0.5f,    1.0f, 0.0f, 0.0f,
		  0.5f,  0.5f, -0.5f,    1.0f, 1.0f, 0.0f,
		 -0.5f,  0.5f, -0.5f,    0.0f, 1.0f, 0.0f,

		 // BOTTOM FACE
		 -0.5f, -0.5f, -0.5f,    0.0f, 0.0f, 5.0f,
		  0.5f, -0.5f, -0.5f,    1.0f, 0.0f, 5.0f,
		  0.5f, -0.5f,  0.5f,    1.0f, 1.0f, 5.0f,

		  0.5f, -0.5f,  0.5f,    1.0f, 1.0f, 5.0f,
		 -0.5f, -0.5f,  0.5f,    0.0f, 1.0f, 5.0f,
		 -0.5f, -0.5f, -0.5f,    0.0f, 0.0f, 5.0f,
	};

	// 1.7
	// Create a Vertex Buffer Object (VBO).
	// A VBO is memory on the graphics card used to store our vertex coordinates.
	glGenBuffers(1, &VBO);

	// 1.7.2
	// Tell OpenGL that this is the buffer we currently want to work with.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// 1.7.3
	// Copy our cube vertex data from normal memory into GPU memory.
	glBufferData(
		GL_ARRAY_BUFFER,
		sizeof(cubeVertices),
		cubeVertices,
		GL_STATIC_DRAW
	);

	// 1.8
	// Create a Vertex Array Object (VAO).
	// The VAO remembers how our vertex data is organized.
	glGenVertexArrays(1, &VAO);

	// 1.8.2
	// Tell OpenGL that this VAO is the one we are currently setting up.
	glBindVertexArray(VAO);

	// 1.8.3
	// Re-bind our VBO so the VAO knows which vertex buffer we are using.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// 1.8.4
	// Tell OpenGL where the XYZ position is stored.
	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		6 * sizeof(float),
		(void*)0
	);

	glEnableVertexAttribArray(0);

	// 1.8.6
	// Tell OpenGL where the texture coordinates are stored.
	glVertexAttribPointer(
		1,
		2,
		GL_FLOAT,
		GL_FALSE,
		6 * sizeof(float),
		(void*)(3 * sizeof(float))
	);

	glEnableVertexAttribArray(1);

	// Face index.
	// 0 = top
	// 1 = front
	// 2 = back
	// 3 = left
	// 4 = right
	// 5 = bottom
	glVertexAttribPointer(
		2,
		1,
		GL_FLOAT,
		GL_FALSE,
		6 * sizeof(float),
		(void*)(5 * sizeof(float))
	);

	glEnableVertexAttribArray(2);

	// 1.9
	// Vertex shader:
	// Takes each vertex's position in the 3D world and converts it
	// into the correct position on our 2D screen.
	//
	// It also receives the texture coordinates and face index.
	const char* vertexShaderSource =
		"#version 330 core\n"

		"layout (location = 0) in vec3 position;\n"
		"layout (location = 1) in vec2 textureCoordinate;\n"
		"layout (location = 2) in float faceIndex;\n"

		"out vec2 texCoord;\n"

		"uniform mat4 model;\n"
		"uniform mat4 view;\n"
		"uniform mat4 projection;\n"

		"uniform float textureRow;\n"
		"uniform float atlasRows;\n"

		"void main()\n"
		"{\n"

		"    gl_Position = projection * view * model * vec4(position, 1.0);\n"

		"    float atlasU = (faceIndex + textureCoordinate.x) / 6.0;\n"
		"    float atlasV = (textureRow + (1.0 - textureCoordinate.y)) / atlasRows;\n"

		"    texCoord = vec2(atlasU, atlasV);\n"

		"}\n";

	// 1.9.1
	// Create a vertex shader on the GPU.
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(
		vertexShader,
		1,
		&vertexShaderSource,
		nullptr
	);

	glCompileShader(vertexShader);

	// 1.10
	// Fragment shader:
	// Controls the color of every pixel drawn on the block.
	const char* fragmentShaderSource =
		"#version 330 core\n"

		"in vec2 texCoord;\n"

		"out vec4 finalColor;\n"

		"uniform sampler2D blockTexture;\n"

		"void main()\n"
		"{\n"

		"    finalColor = texture(blockTexture, texCoord);\n"

		"}\n";

	// 1.10.1
	// Create a fragment shader on the GPU.
	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(
		fragmentShader,
		1,
		&fragmentShaderSource,
		nullptr
	);

	glCompileShader(fragmentShader);

	// 1.11
	// Create a shader program.
	shaderProgram = glCreateProgram();

	// 1.11.1
	// Attach both shaders.
	glAttachShader(
		shaderProgram,
		vertexShader
	);

	glAttachShader(
		shaderProgram,
		fragmentShader
	);

	// 1.11.2
	// Link them together.
	glLinkProgram(shaderProgram);

	// 1.11.3
	// The separate shaders are no longer needed after linking.
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void Renderer::bindCube()
{
	glBindVertexArray(VAO);
}

unsigned int Renderer::getShaderProgram() const
{
	return shaderProgram;
}