#pragma once

class Renderer
{
public:
	Renderer();

	void bindCube();

private:
	unsigned int VBO;
	unsigned int VAO;
};