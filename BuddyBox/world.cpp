#include "world.h"

#include <cmath>
#include <limits>
#include <fstream>
#include <string>


// ============================================================
// Block lookup
// ============================================================

bool World::hasBlock(
    int x,
    int y,
    int z
) const
{
    auto position = std::make_tuple(x, y, z);

    return blocks.find(position) != blocks.end();
}


bool World::isSolidAt(
    int x,
    int y,
    int z
) const
{
    auto position = std::make_tuple(x, y, z);
    auto block = blocks.find(position);

    // Empty space is not solid.
    if (block == blocks.end())
    {
        return false;
    }

    return block->second.solid;
}


// ============================================================
// Place a block
// ============================================================

void World::placeBlock(
    int x,
    int y,
    int z,
    const Block& block
)
{
    auto position = std::make_tuple(x, y, z);

    blocks.insert_or_assign(position, block);

    // Only special blocks need ongoing updates.
    if (block.needsUpdate())
    {
        activeBlocks.insert(position);
    }
    else
    {
        activeBlocks.erase(position);
    }
}


// ============================================================
// Remove a block
// ============================================================

void World::removeBlock(
    int x,
    int y,
    int z
)
{
    auto position = std::make_tuple(x, y, z);

    blocks.erase(position);
    activeBlocks.erase(position);
}


// ============================================================
// Block raycasting
//
// Visit every crossed grid cell in order.
//
// hitX/Y/Z:
//     The block that was hit.
//
// previousX/Y/Z:
//     The cell immediately before the hit.
//     Used to position a new block beside the target.
// ============================================================

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
    // Normalize the direction so distance uses world units.
    const double length = glm::length(glm::dvec3(direction));

    if (
        !std::isfinite(length) ||
        length <= 0.0 ||
        !std::isfinite(maxDistance) ||
        maxDistance < 0.0f
        )
    {
        return false;
    }

    const glm::dvec3 ray = glm::dvec3(direction) / length;

    glm::ivec3 cell;
    glm::ivec3 step;

    glm::dvec3 nextBoundary;
    glm::dvec3 boundarySpacing;

    const double infinity =
        std::numeric_limits<double>::infinity();

    // Prepare traversal for X, Y, and Z.
    for (int axis = 0; axis < 3; ++axis)
    {
        if (!std::isfinite(origin[axis]))
        {
            return false;
        }

        // Blocks are centered on whole-number coordinates.
        cell[axis] = static_cast<int>(
            std::floor(origin[axis] + 0.5)
            );

        // +1 for forward, -1 for backward, 0 for no movement.
        step[axis] =
            (ray[axis] > 0.0) - (ray[axis] < 0.0);

        if (step[axis] == 0)
        {
            // This ray never crosses a boundary on this axis.
            nextBoundary[axis] = infinity;
            boundarySpacing[axis] = infinity;
        }
        else
        {
            const double edge =
                cell[axis] + (step[axis] > 0 ? 0.5 : -0.5);

            // Distance to the first boundary.
            nextBoundary[axis] =
                (edge - origin[axis]) / ray[axis];

            // Distance between subsequent boundaries.
            boundarySpacing[axis] =
                1.0 / std::abs(ray[axis]);
        }
    }

    glm::ivec3 previous = cell;

    while (true)
    {
        if (hasBlock(cell.x, cell.y, cell.z))
        {
            hitX = cell.x;
            hitY = cell.y;
            hitZ = cell.z;

            previousX = previous.x;
            previousY = previous.y;
            previousZ = previous.z;

            return true;
        }

        // Choose the closest upcoming boundary.
        // Exact ties are handled in X, Y, Z order.
        int axis = 0;

        if (nextBoundary.y < nextBoundary[axis])
        {
            axis = 1;
        }

        if (nextBoundary.z < nextBoundary[axis])
        {
            axis = 2;
        }

        // Stop before moving beyond the player's reach.
        if (nextBoundary[axis] > maxDistance)
        {
            return false;
        }

        // Cross one face at a time.
        // This keeps the placement cell beside the hit block.
        previous = cell;
        cell[axis] += step[axis];
        nextBoundary[axis] += boundarySpacing[axis];
    }
}


// ============================================================
// Load a world
//
// File format:
// X Y Z BlockType
//
// Example:
// 0 0 0 Grass
// 1 0 0 Stone
// ============================================================

bool World::loadFromFile(
    const std::string& filename
)
{
    std::ifstream file(filename);

    if (!file.is_open())
    {
        return false;
    }

    int x;
    int y;
    int z;

    std::string blockTypeName;

    while (file >> x >> y >> z >> blockTypeName)
    {
        BlockType blockType;

        if (blockTypeName == "Grass")
        {
            blockType = BlockType::Grass;
        }
        else if (blockTypeName == "Spawner")
        {
            blockType = BlockType::Spawner;
        }
        else if (blockTypeName == "Dirt")
        {
            blockType = BlockType::Dirt;
        }
        else if (blockTypeName == "Wood")
        {
            blockType = BlockType::Wood;
        }
        else if (blockTypeName == "Leaf")
        {
            blockType = BlockType::Leaf;
        }
        else if (blockTypeName == "Stone")
        {
            blockType = BlockType::Stone;
        }
        else
        {
            // Skip unknown block types.
            continue;
        }

        Block block(blockType);

        placeBlock(x, y, z, block);
    }

    return true;
}