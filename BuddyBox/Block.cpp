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
	}
}