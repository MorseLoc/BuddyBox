#include "UIRenderer.h"

#include <glad/glad.h>

#include <iostream>


UIRenderer::UIRenderer()
{
	VAO = 0;
	VBO = 0;
	shaderProgram = 0;
}


bool UIRenderer::initialize()
{
	// Two triangles forming one rectangle.
	float vertices[] =
	{
		// position   // texture coordinates
		-1.0f, -1.0f, 0.0f, 0.0f,
		 1.0f, -1.0f, 1.0f, 0.0f,
		 1.0f,  1.0f, 1.0f, 1.0f,

		-1.0f, -1.0f, 0.0f, 0.0f,
		 1.0f,  1.0f, 1.0f, 1.0f,
		-1.0f,  1.0f, 0.0f, 1.0f
	};


	glGenVertexArrays(
		1,
		&VAO
	);

	glGenBuffers(
		1,
		&VBO
	);


	glBindVertexArray(
		VAO
	);

	glBindBuffer(
		GL_ARRAY_BUFFER,
		VBO
	);

	glBufferData(
		GL_ARRAY_BUFFER,
		sizeof(vertices),
		vertices,
		GL_STATIC_DRAW
	);


	// Position data.
	glVertexAttribPointer(
		0,
		2,
		GL_FLOAT,
		GL_FALSE,
		4 * sizeof(float),
		(void*)0
	);

	glEnableVertexAttribArray(
		0
	);


	// Texture-coordinate data.
	glVertexAttribPointer(
		1,
		2,
		GL_FLOAT,
		GL_FALSE,
		4 * sizeof(float),
		(void*)(2 * sizeof(float))
	);

	glEnableVertexAttribArray(
		1
	);


	const char* vertexShaderSource = R"(
	#version 330 core

	layout(location = 0) in vec2 position;
	layout(location = 1) in vec2 textureCoordinate;

	out vec2 uv;

	uniform vec2 uiScale;
	uniform vec2 uiPosition;

	void main()
	{
		vec2 screenPosition =
			position * uiScale + uiPosition;

		gl_Position =
			vec4(screenPosition, 0.0, 1.0);

		uv = textureCoordinate;
	}
)";


	const char* fragmentShaderSource = R"(
	#version 330 core

	in vec2 uv;

	out vec4 finalColor;

	uniform sampler2D uiTexture;

	// 0 = draw hotbar
	// 1 = draw block icon
	// 2 = draw solid white shape
	uniform int drawMode;

	// Hotbar frame information.
	uniform float frameStart;
	uniform float frameHeight;

	// Block atlas information.
	uniform float textureRow;
	uniform float atlasRows;

	void main()
	{
		// Draw the hotbar frame.
		if (drawMode == 0)
		{
			vec2 frameUV = uv;

			frameUV.y =
				frameStart +
				uv.y * frameHeight;

			finalColor =
				texture(uiTexture, frameUV);
		}

		// Draw the FRONT face of a block.
		else if (drawMode == 1)
		{
			// Face 1 = front face.
			float atlasU =
				(1.0 + uv.x) / 6.0;

			float atlasV =
				(textureRow + (1.0 - uv.y))
				/ atlasRows;

			finalColor =
				texture(
					uiTexture,
					vec2(atlasU, atlasV)
				);
		}

		// Draw a solid white shape.
		else if (drawMode == 2)
		{
			finalColor = vec4(
				1.0,
				1.0,
				1.0,
				1.0
			);
		}
	}
)";


	unsigned int vertexShader =
		glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(
		vertexShader,
		1,
		&vertexShaderSource,
		nullptr
	);

	glCompileShader(
		vertexShader
	);


	unsigned int fragmentShader =
		glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(
		fragmentShader,
		1,
		&fragmentShaderSource,
		nullptr
	);

	glCompileShader(
		fragmentShader
	);


	shaderProgram =
		glCreateProgram();

	glAttachShader(
		shaderProgram,
		vertexShader
	);

	glAttachShader(
		shaderProgram,
		fragmentShader
	);

	glLinkProgram(
		shaderProgram
	);


	glDeleteShader(
		vertexShader
	);

	glDeleteShader(
		fragmentShader
	);


	return true;
}


void UIRenderer::drawHotbar(
	unsigned int hotbarTexture,
	unsigned int blockAtlasTexture,
	int selectedSlot,
	const Inventory& inventory,
	int atlasRows
)
{
	glUseProgram(
		shaderProgram
	);

	int drawModeLocation =
		glGetUniformLocation(
			shaderProgram,
			"drawMode"
		);

	glUniform1i(
		drawModeLocation,
		0
	);


	// UI should draw over the 3D world.
	glDisable(
		GL_DEPTH_TEST
	);


	glActiveTexture(
		GL_TEXTURE0
	);

	glBindTexture(
		GL_TEXTURE_2D,
		hotbarTexture
	);


	int textureLocation =
		glGetUniformLocation(
			shaderProgram,
			"uiTexture"
		);

	glUniform1i(
		textureLocation,
		0
	);


	// Make the hotbar smaller than the screen.
	int scaleLocation =
		glGetUniformLocation(
			shaderProgram,
			"uiScale"
		);

	glUniform2f(
		scaleLocation,
		0.45f,
		0.075f
	);


	// Put it near the bottom center.
	int positionLocation =
		glGetUniformLocation(
			shaderProgram,
			"uiPosition"
		);

	glUniform2f(
		positionLocation,
		0.0f,
		-0.85f
	);


	// Each of our 6 frames takes up
	// one sixth of the texture.
	float frameHeight =
		1.0f / 6.0f;


	float frameStart =
		static_cast<float>(selectedSlot)
		* frameHeight;


	int frameStartLocation =
		glGetUniformLocation(
			shaderProgram,
			"frameStart"
		);

	glUniform1f(
		frameStartLocation,
		frameStart
	);


	int frameHeightLocation =
		glGetUniformLocation(
			shaderProgram,
			"frameHeight"
		);

	glUniform1f(
		frameHeightLocation,
		frameHeight
	);


	glBindVertexArray(
		VAO
	);

	glDrawArrays(
		GL_TRIANGLES,
		0,
		6
	);

	// --------------------------------------------------
// DRAW THE SIX BLOCK ICONS
// --------------------------------------------------

// Switch the UI shader into block-icon mode.
	glUniform1i(
		drawModeLocation,
		1
	);


	// Use artdex.png instead of ScrollWheel.png.
	glBindTexture(
		GL_TEXTURE_2D,
		blockAtlasTexture
	);


	// Tell the shader how many block rows
	// exist inside artdex.png.
	int atlasRowsLocation =
		glGetUniformLocation(
			shaderProgram,
			"atlasRows"
		);

	glUniform1f(
		atlasRowsLocation,
		static_cast<float>(atlasRows)
	);


	// Make each block icon small enough
	// to fit inside one hotbar box.
	glUniform2f(
		scaleLocation,
		0.055f,
		0.065f
	);


	// Draw one icon for each of the six slots.
	for (int slot = 0; slot < 6; slot++)
	{
		// Ask Inventory which block is stored here.
		BlockType blockType =
			inventory.getBlockTypeAtSlot(slot);


		// Create a temporary Block so we can get
		// its texture row in artdex.png.
		Block block(blockType);


		// Tell the shader which block row to use.
		int textureRowLocation =
			glGetUniformLocation(
				shaderProgram,
				"textureRow"
			);

		glUniform1f(
			textureRowLocation,
			static_cast<float>(block.textureRow)
		);


		// Calculate where this slot sits on the hotbar.
		float slotX =
			-0.375f +
			(static_cast<float>(slot) * 0.15f);


		// Put the icon inside its hotbar box.
		glUniform2f(
			positionLocation,
			slotX,
			-0.85f
		);


		// Draw the icon.
		glDrawArrays(
			GL_TRIANGLES,
			0,
			6
		);
	}

	


	// Turn normal 3D depth testing back on.
	glEnable(
		GL_DEPTH_TEST
	);
}

void UIRenderer::drawCrosshair()
{
	glUseProgram(shaderProgram);

	glDisable(GL_DEPTH_TEST);

	// We are not sampling a texture here,
	// so use a plain white color instead.
	glBindTexture(
		GL_TEXTURE_2D,
		0
	);

	int scaleLocation =
		glGetUniformLocation(
			shaderProgram,
			"uiScale"
		);

	int positionLocation =
		glGetUniformLocation(
			shaderProgram,
			"uiPosition"
		);

	int drawModeLocation =
		glGetUniformLocation(
			shaderProgram,
			"drawMode"
		);

	// We'll add a simple solid-color mode next.
	glUniform1i(
		drawModeLocation,
		2
	);

	glBindVertexArray(VAO);

	// Vertical bar.
	glUniform2f(
		scaleLocation,
		0.008f,
		0.035f
	);

	glUniform2f(
		positionLocation,
		0.0f,
		0.0f
	);

	glDrawArrays(
		GL_TRIANGLES,
		0,
		6
	);

	// Horizontal bar.
	glUniform2f(
		scaleLocation,
		0.025f,
		0.010f
	);

	glUniform2f(
		positionLocation,
		0.0f,
		0.0f
	);

	glDrawArrays(
		GL_TRIANGLES,
		0,
		6
	);

	glEnable(GL_DEPTH_TEST);
}