#include "Block.h"

// Creates a block and gives it the
// correct properties for its block type.
Block::Block(BlockType blockType)
{
	// Store what kind of block this is.
	type = blockType;


	// GRASS BLOCK
	if (type == BlockType::Grass)
	{
		// Grass blocks are normal 1 x 1 x 1 blocks.
		size = glm::vec3(
			1.0f,
			1.0f,
			1.0f
		);

		// The player cannot walk through grass blocks.
		solid = true;

		// Grass blocks do not spawn Jebubs.
		spawnsJebub = false;

		// Grass uses the grass texture.
		texturePath = "textures/grass.png";
	}

	// SPAWNER BLOCK
	if (type == BlockType::Spawner)
	{
		// Spawners are normal 1 x 1 x 1 blocks.
		size = glm::vec3(
			1.0f,
			1.0f,
			1.0f
		);

		// The player cannot walk through a Spawner.
		solid = true;

		// This block is allowed to create Jebubs.
		spawnsJebub = true;

		// Temporary texture.
		// We can make a real Spawner texture next.
		texturePath = "textures/spawner.png";
	}
}