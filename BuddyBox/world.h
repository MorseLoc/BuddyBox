#pragma once

#include <map>
#include <tuple>
#include <string>

#include "Block.h"

#include <glm/glm.hpp>

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

	// Removes a block from this grid position.
	void removeBlock(
		int x,
		int y,
		int z
	);

	// Finds the first block hit by a ray.
	//
	// origin = where the ray starts
	// direction = which way the ray travels
	// maxDistance = how far the ray can reach
	//
	// Returns true if a block was found.
	bool raycastBlock(
		const glm::vec3& origin,
		const glm::vec3& direction,
		float maxDistance,
		int& hitX,
		int& hitY,
		int& hitZ,
		int& previousX,
		int& previousY,
		int& previousZ
	) const;

	// Loads a world from a text file.
	bool loadFromFile(
		const std::string& filename
	);
};