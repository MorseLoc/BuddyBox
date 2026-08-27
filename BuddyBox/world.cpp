#include "World.h"

#include <cmath>
#include <fstream>
#include <string>


// ============================================================
// Block lookup
// ============================================================

// Returns true if any block exists at this grid position.
bool World::hasBlock(
    int x,
    int y,
    int z
) const
{
    auto position =
        std::make_tuple(
            x,
            y,
            z
        );


    return blocks.find(position) !=
        blocks.end();
}


// Returns true only if a solid block exists
// at this grid position.
bool World::isSolidAt(
    int x,
    int y,
    int z
) const
{
    auto position =
        std::make_tuple(
            x,
            y,
            z
        );


    auto block =
        blocks.find(position);


    // Empty space is not solid.
    if (block == blocks.end())
    {
        return false;
    }


    return block->second.solid;
}


// ============================================================
// Block editing
// ============================================================

// Places a block at a grid position.
//
// If a block already exists there,
// insert_or_assign replaces it.
void World::placeBlock(
    int x,
    int y,
    int z,
    const Block& block
)
{
    auto position =
        std::make_tuple(
            x,
            y,
            z
        );


    blocks.insert_or_assign(
        position,
        block
    );
}


// Removes the block at a grid position.
//
// If no block exists there,
// erase() simply does nothing.
void World::removeBlock(
    int x,
    int y,
    int z
)
{
    auto position =
        std::make_tuple(
            x,
            y,
            z
        );


    blocks.erase(
        position
    );
}


// ============================================================
// Block raycasting
//
// Shoots an invisible ray through the world and finds
// the first block it touches.
//
// Used for breaking and placing blocks.
//
// hitX/Y/Z:
//     The block that was hit.
//
// previousX/Y/Z:
//     The empty grid position immediately before the hit block.
//     This is where a newly placed block can go.
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
    // Distance between each ray test.
    //
    // Smaller values are more precise,
    // but require more checks.
    const float stepSize =
        0.05f;


    // Start the ray at the camera position.
    glm::vec3 rayPosition =
        origin;


    // Remember the grid cell the ray was previously inside.
    int lastX =
        static_cast<int>(
            std::floor(rayPosition.x + 0.5f)
            );

    int lastY =
        static_cast<int>(
            std::floor(rayPosition.y + 0.5f)
            );

    int lastZ =
        static_cast<int>(
            std::floor(rayPosition.z + 0.5f)
            );


    // Move along the ray until it reaches maxDistance.
    for (
        float distance = 0.0f;
        distance <= maxDistance;
        distance += stepSize
        )
    {
        // Calculate the ray's position at this distance.
        rayPosition =
            origin +
            direction * distance;


        // Convert that position into a block-grid coordinate.
        int x =
            static_cast<int>(
                std::floor(rayPosition.x + 0.5f)
                );

        int y =
            static_cast<int>(
                std::floor(rayPosition.y + 0.5f)
                );

        int z =
            static_cast<int>(
                std::floor(rayPosition.z + 0.5f)
                );


        // The first block encountered is the block we hit.
        if (hasBlock(x, y, z))
        {
            hitX = x;
            hitY = y;
            hitZ = z;


            // Save the previous grid cell too.
            //
            // This lets block placement put a new block
            // directly beside the block that was hit.
            previousX = lastX;
            previousY = lastY;
            previousZ = lastZ;


            return true;
        }


        // Remember this grid cell for the next ray step.
        lastX = x;
        lastY = y;
        lastZ = z;
    }


    // The ray reached maxDistance without hitting anything.
    return false;
}


// ============================================================
// World loading
//
// Loads blocks from a text file.
//
// Each line uses:
//
// X Y Z BlockType
//
// Example:
//
// 0 0 0 Grass
// 1 0 0 Stone
// ============================================================

bool World::loadFromFile(
    const std::string& filename
)
{
    std::ifstream file(
        filename
    );


    // Loading fails if the file cannot be opened.
    if (!file.is_open())
    {
        return false;
    }


    int x;
    int y;
    int z;

    std::string blockTypeName;


    // Read one block definition at a time.
    while (
        file >>
        x >>
        y >>
        z >>
        blockTypeName
        )
    {
        // ----------------------------------------------------
        // Convert the text name into a BlockType
        // ----------------------------------------------------

        BlockType blockType;


        if (blockTypeName == "Grass")
        {
            blockType =
                BlockType::Grass;
        }
        else if (blockTypeName == "Spawner")
        {
            blockType =
                BlockType::Spawner;
        }
        else if (blockTypeName == "Dirt")
        {
            blockType =
                BlockType::Dirt;
        }
        else if (blockTypeName == "Wood")
        {
            blockType =
                BlockType::Wood;
        }
        else if (blockTypeName == "Leaf")
        {
            blockType =
                BlockType::Leaf;
        }
        else if (blockTypeName == "Stone")
        {
            blockType =
                BlockType::Stone;
        }
        else
        {
            // Unknown block type.
            //
            // Ignore this line instead of creating
            // a block with invalid properties.
            continue;
        }


        // ----------------------------------------------------
        // Create and place the block
        // ----------------------------------------------------

        Block block(
            blockType
        );


        placeBlock(
            x,
            y,
            z,
            block
        );
    }


    return true;
}