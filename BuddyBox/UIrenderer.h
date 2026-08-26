#pragma once

#include "inventory.h"

class UIRenderer
{
public:

	UIRenderer();

	// Sets up everything needed to draw 2D UI.
	bool initialize();

	void drawHotbar(
		unsigned int hotbarTexture,
		unsigned int blockAtlasTexture,
		int selectedSlot,
		const Inventory& inventory,
		int atlasRows
	);

	void drawCrosshair();

private:

	unsigned int VAO;
	unsigned int VBO;
	unsigned int shaderProgram;
};