#pragma once

class Renderer
{
public:
	Renderer();

	void bindCube();

	unsigned int getShaderProgram() const;

private:
	unsigned int VBO;
	unsigned int VAO;
	unsigned int shaderProgram;
};