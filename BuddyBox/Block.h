#pragma once

#include <glm/glm.hpp>

// Lists all types of blocks that can exist in the game.
//
// For now, Grass is the only block type.
enum class BlockType
{
	Grass
};


// Represents one block in the world.
struct Block
{
	// What kind of block this is.
	BlockType type;

	// How large the block is.
	//
	// Normal blocks are 1 x 1 x 1.
	glm::vec3 size;

	// Whether this block has solid collision.
	bool solid;

	// Creates a block of the given type.
	Block(BlockType type);
};