#pragma once

#include <map>
#include <tuple>
#include <string>

#include "Block.h"


// Represents the block world.
struct World
{
	// Stores every block using its grid coordinate.
	//
	// Example:
	// (0, 0, 0) -> Grass
	// (1, 0, 0) -> Grass
	std::map<
		std::tuple<int, int, int>,
		Block
	> blocks;


	// Checks whether ANY block exists
	// at this grid position.
	bool hasBlock(
		int x,
		int y,
		int z
	) const;


	// Checks whether a SOLID block exists
	// at this grid position.
	bool isSolidAt(
		int x,
		int y,
		int z
	) const;


	// Places a block at this grid position.
	void placeBlock(
		int x,
		int y,
		int z,
		const Block& block
	);


	// Loads a world from a text file.
	bool loadFromFile(
		const std::string& filename
	);
};