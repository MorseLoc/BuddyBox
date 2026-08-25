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

		// Grass uses row 0 of artdex.png.
		textureRow = 0;
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

		// Spawner uses row 1 of artdex.png.
		textureRow = 1;
	}

	// DIRT BLOCK
	if (type == BlockType::Dirt)
	{
		// DIRT blocks are normal 1 x 1 x 1 blocks.
		size = glm::vec3(
			1.0f,
			1.0f,
			1.0f
		);

		// The player cannot walk through dirt blocks.
		solid = true;

		// DIRT blocks do not spawn Jebubs.
		spawnsJebub = false;

		// DIRT uses row 2 of artdex.png.
		textureRow = 2;
	}

	// Wood BLOCK
	if (type == BlockType::Wood)
	{
		// Wood blocks are normal 1 x 1 x 1 blocks.
		size = glm::vec3(
			1.0f,
			1.0f,
			1.0f
		);

		// The player cannot walk through wood blocks.
		solid = true;

		// Wood blocks do not spawn Jebubs.
		spawnsJebub = false;

		// Wood uses row 3 of artdex.png.
		textureRow = 3;
	}

	// Leaf BLOCK
	if (type == BlockType::Leaf)
	{
		// Leaf blocks are normal 1 x 1 x 1 blocks.
		size = glm::vec3(
			1.0f,
			1.0f,
			1.0f
		);

		// The player cannot walk through leaf blocks.
		solid = true;

		// Leaf blocks do not spawn Jebubs.
		spawnsJebub = false;

		// Leaf uses row 4 of artdex.png.
		textureRow = 4;
	}

	// Stone BLOCK
	if (type == BlockType::Stone)
	{
		// Stone blocks are normal 1 x 1 x 1 blocks.
		size = glm::vec3(
			1.0f,
			1.0f,
			1.0f
		);

		// The player cannot walk through stone blocks.
		solid = true;

		// Stone blocks do not spawn Jebubs.
		spawnsJebub = false;

		// Stone uses row 5 of artdex.png.
		textureRow = 5;
	}
}