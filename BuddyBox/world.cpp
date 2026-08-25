#include "World.h"

#include <fstream>
#include <string>

#include <cmath>

// Checks whether the world already contains a block
// at the given X, Y, Z grid position.
bool World::hasBlock(
	int x,
	int y,
	int z
) const
{
	// Create one grid coordinate from X, Y, and Z.
	auto position = std::make_tuple(
		x,
		y,
		z
	);

	// Search the block map for that position.
	return blocks.find(position) != blocks.end();
}

// Checks whether a solid block exists
// at the given X, Y, Z grid position.
bool World::isSolidAt(
	int x,
	int y,
	int z
) const
{
	// Create the grid coordinate we want to check.
	auto position = std::make_tuple(
		x,
		y,
		z
	);

	// Try to find a block at this position.
	auto block = blocks.find(position);

	// If no block exists here, the space is not solid.
	if (block == blocks.end())
	{
		return false;
	}

	// If a block does exist, return its solid property.
	return block->second.solid;
}

// Places a block at the given grid position.
void World::placeBlock(
	int x,
	int y,
	int z,
	const Block& block
)
{
	// Create one grid coordinate from X, Y, and Z.
	auto position = std::make_tuple(
		x,
		y,
		z
	);

	// Insert the block into the map at that position.
	blocks.insert_or_assign(position, block);
}

// Removes a block from the given grid position.
void World::removeBlock(
	int x,
	int y,
	int z
)
{
	auto position = std::make_tuple(
		x,
		y,
		z
	);

	blocks.erase(position);
}

// Finds the first block hit by a ray.
bool World::raycastBlock(
	const glm::vec3& origin,
	const glm::vec3& direction,
	float maxDistance,
	int& hitX,
	int& hitY,
	int& hitZ,
	int& previousX,
	int& previousY,
	int& previousZ
) const
{
	const float stepSize = 0.05f;

	glm::vec3 rayPosition = origin;

	int lastX = static_cast<int>(floor(rayPosition.x + 0.5f));
	int lastY = static_cast<int>(floor(rayPosition.y + 0.5f));
	int lastZ = static_cast<int>(floor(rayPosition.z + 0.5f));

	for (float distance = 0.0f;
		distance <= maxDistance;
		distance += stepSize)
	{
		rayPosition = origin + direction * distance;

		int x = static_cast<int>(floor(rayPosition.x + 0.5f));
		int y = static_cast<int>(floor(rayPosition.y + 0.5f));
		int z = static_cast<int>(floor(rayPosition.z + 0.5f));

		if (hasBlock(x, y, z))
		{
			hitX = x;
			hitY = y;
			hitZ = z;

			previousX = lastX;
			previousY = lastY;
			previousZ = lastZ;

			return true;
		}

		lastX = x;
		lastY = y;
		lastZ = z;
	}

	return false;
}

// Loads blocks from a world file.
//
// Expected format for each line:
// X Y Z BlockType
//
// Example:
// 0 0 0 Grass
bool World::loadFromFile(const std::string& filename)
{
	std::ifstream file(filename);

	// If the file could not be opened, loading failed.
	if (!file.is_open())
	{
		return false;
	}

	int x;
	int y;
	int z;
	std::string blockTypeName;

	// Read one block at a time until the file ends.
	while (file >> x >> y >> z >> blockTypeName)
	{
		if (blockTypeName == "Grass")
		{
			Block block(BlockType::Grass);

			placeBlock(
				x,
				y,
				z,
				block
			);
		}

		else if (blockTypeName == "Spawner")
		{
			Block block(BlockType::Spawner);

			placeBlock(
				x,
				y,
				z,
				block
			);
		}

		else if (blockTypeName == "Dirt")
		{
			Block block(BlockType::Dirt);

			placeBlock(
				x,
				y,
				z,
				block
			);
		}

		else if (blockTypeName == "Wood")
		{
			Block block(BlockType::Wood);

			placeBlock(
				x,
				y,
				z,
				block
			);
		}

		else if (blockTypeName == "Leaf")
		{
			Block block(BlockType::Leaf);
			placeBlock(
				x,
				y,
				z,
				block
			);
		}

		else if (blockTypeName == "Stone")
		{
			Block block(BlockType::Stone);
			placeBlock(
				x,
				y,
				z,
				block
			);
		}
	}

	return true;
}