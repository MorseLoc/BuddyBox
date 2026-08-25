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
}

void Renderer::bindCube()
{
	glBindVertexArray(VAO);
}
