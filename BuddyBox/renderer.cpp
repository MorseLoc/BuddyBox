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
		-0.5f, -0.5f,  0.5f,    0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,    1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,    1.0f, 1.0f,

		 0.5f,  0.5f,  0.5f,    1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,    0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,    0.0f, 0.0f,

		// BACK FACE
		-0.5f, -0.5f, -0.5f,    1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,    1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,    0.0f, 1.0f,

		 0.5f,  0.5f, -0.5f,    0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,    0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,    1.0f, 0.0f,

		// LEFT FACE
		-0.5f,  0.5f,  0.5f,    1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,    0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,    0.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,    0.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,    1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,    1.0f, 1.0f,

		// RIGHT FACE
		 0.5f,  0.5f,  0.5f,    0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,    0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,    1.0f, 0.0f,

		 0.5f, -0.5f, -0.5f,    1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,    1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,    0.0f, 1.0f,

		 // TOP FACE
		 -0.5f,  0.5f, -0.5f,    0.0f, 1.0f,
		 -0.5f,  0.5f,  0.5f,    0.0f, 0.0f,
		  0.5f,  0.5f,  0.5f,    1.0f, 0.0f,

		  0.5f,  0.5f,  0.5f,    1.0f, 0.0f,
		  0.5f,  0.5f, -0.5f,    1.0f, 1.0f,
		 -0.5f,  0.5f, -0.5f,    0.0f, 1.0f,

		 // BOTTOM FACE
		 -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,
		  0.5f, -0.5f, -0.5f,    1.0f, 0.0f,
		  0.5f, -0.5f,  0.5f,    1.0f, 1.0f,

		  0.5f, -0.5f,  0.5f,    1.0f, 1.0f,
		 -0.5f, -0.5f,  0.5f,    0.0f, 1.0f,
		 -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,
	};

	// 1.7
	// Create a Vertex Buffer Object (VBO).
	// A VBO is memory on the graphics card used to store our vertex coordinates.
	// unsigned int VBO;

	//1.7.1
	// Ask OpenGL to create one buffer and store its ID inside VBO.
	glGenBuffers(1, &VBO);

	//1.7.2
	// Tell OpenGL that this is the buffer we currently want to work with.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//1.7.3
	// Copy our square vertex data from normal memory into GPU memory.
	glBufferData(
		GL_ARRAY_BUFFER,
		sizeof(cubeVertices),
		cubeVertices,
		GL_STATIC_DRAW
	);

	// 1.8
	// Create a Vertex Array Object (VAO).
	// The VAO remembers how our vertex data is organized.
	// unsigned int VAO;

	// 1.8.1
	// Ask OpenGL to create one VAO and store its ID.
	glGenVertexArrays(1, &VAO);

	// 1.8.2
	// Tell OpenGL that this VAO is the one we are currently setting up.
	glBindVertexArray(VAO);


	// 1.8.3
	// Re-bind our VBO so the VAO knows which vertex buffer we are using.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// 1.8.4
	// Tell OpenGL how to read each vertex.
	//
	// 0 = vertex attribute slot 0
	// 3 = each vertex has 3 numbers: X, Y, Z
	// GL_FLOAT = each number is a float
	// GL_FALSE = do not normalize the numbers
	// 3 * sizeof(float) = move forward 3 floats to reach the next vertex
	// (void*)0 = start reading at the beginning of each vertex
	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		5 * sizeof(float),
		(void*)0
	);

	// 1.8.5
	// Turn on vertex attribute slot 0.
	glEnableVertexAttribArray(0);

	// 1.8.6
	// Tell OpenGL where the texture coordinates are stored.
	//
	// Attribute slot 1
	// 2 values per vertex: U and V
	// Each vertex contains 5 floats total
	// Texture data starts after the first 3 floats
	glVertexAttribPointer(
		1,
		2,
		GL_FLOAT,
		GL_FALSE,
		5 * sizeof(float),
		(void*)(3 * sizeof(float))
	);

	glEnableVertexAttribArray(1);

	// 1.9
		// Vertex shader:
		// Takes each vertex's position in the 3D world and converts it
		// into the correct position on our 2D screen.
		//
		// It also receives the texture coordinates (U and V)
		// and passes them to the fragment shader.
	const char* vertexShaderSource =
		"#version 330 core\n"

		// The XYZ position coming from our VAO/VBO.
		"layout (location = 0) in vec3 position;\n"

		// The UV texture coordinates coming from our VAO/VBO.
		"layout (location = 1) in vec2 textureCoordinate;\n"

		// Pass the texture coordinates to the fragment shader.
		"out vec2 texCoord;\n"

		// These three matrices control how the object is viewed.
		"uniform mat4 model;\n"
		"uniform mat4 view;\n"
		"uniform mat4 projection;\n"

		"void main()\n"
		"{\n"

		// Convert the vertex from 3D world space into screen space.
		"    gl_Position = projection * view * model * vec4(position, 1.0);\n"

		// Pass this vertex's texture coordinates to the fragment shader.
		"    texCoord = textureCoordinate;\n"

		"}\n";

	// 1.9.1
	// Create a vertex shader on the GPU.
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);

	// Give our shader source code to the GPU.
	glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);

	// Compile the shader into something the GPU can execute.
	glCompileShader(vertexShader);

	// 1.10
	// Fragment shader:
	// Controls the color of every pixel drawn on the block.
	//
	// The texture coordinates come from the vertex shader.
	// "blockTexture" is the texture image we want to draw.
	const char* fragmentShaderSource =
		"#version 330 core\n"

		// Receive the UV texture coordinates from the vertex shader.
		"in vec2 texCoord;\n"

		// The final color that will appear on the screen.
		"out vec4 finalColor;\n"

		// The texture image that C++ will give to this shader.
		"uniform sampler2D blockTexture;\n"

		"void main()\n"
		"{\n"

		// Look at the texture using the UV coordinates
		// and use that pixel as the final color.
		"    finalColor = texture(blockTexture, texCoord);\n"

		"}\n";

	// 1.10.1
	// Create a fragment shader on the GPU.
	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	// Give the fragment shader its source code.
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);

	// Compile the fragment shader so the GPU can execute it.
	glCompileShader(fragmentShader);

	// 1.11
	// Create a shader program.
	// This combines the vertex shader and fragment shader into one usable program.
	// unsigned int shaderProgram;
	shaderProgram = glCreateProgram();

	// 1.11.1
	// Attach both shaders to the program.
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	// 1.11.2
	// Link them together into one complete shader program.
	glLinkProgram(shaderProgram);

	// 1.11.3
	// The separate shader objects are no longer needed after linking.
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
