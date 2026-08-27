#include "Block.h"


// ============================================================
// Block constructor
//
// Creates a block and gives it the correct properties
// based on its BlockType.
//
// Most blocks currently share the same basic properties:
// - 1 x 1 x 1 size
// - solid
// - does not spawn Jebubs
//
// After setting those defaults, we only change the
// properties that are unique to each block type.
// ============================================================

Block::Block(BlockType blockType)
{
    // Store what kind of block this is.
    type = blockType;


    // --------------------------------------------------------
    // Default block properties
    // --------------------------------------------------------

    // All current blocks are normal 1 x 1 x 1 cubes.
    size = glm::vec3(
        1.0f,
        1.0f,
        1.0f
    );


    // All current blocks have collision.
    // This means the player cannot walk through them.
    solid = true;


    // Most blocks do not spawn Jebubs.
    spawnsJebub = false;


    // --------------------------------------------------------
    // Block-specific properties
    // --------------------------------------------------------

    if (type == BlockType::Grass)
    {
        // Grass uses row 0 of the texture atlas.
        textureRow = 0;
    }
    else if (type == BlockType::Spawner)
    {
        // Spawner uses row 1 of the texture atlas.
        textureRow = 1;

        // Spawners are the only current blocks
        // that are allowed to create Jebubs.
        spawnsJebub = true;
    }
    else if (type == BlockType::Dirt)
    {
        // Dirt uses row 2 of the texture atlas.
        textureRow = 2;
    }
    else if (type == BlockType::Wood)
    {
        // Wood uses row 3 of the texture atlas.
        textureRow = 3;
    }
    else if (type == BlockType::Leaf)
    {
        // Leaf uses row 4 of the texture atlas.
        textureRow = 4;
    }
    else if (type == BlockType::Stone)
    {
        // Stone uses row 5 of the texture atlas.
        textureRow = 5;
    }
}