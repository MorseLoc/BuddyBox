#include "Block.h"
#include "NPC.h"




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

    // Default block durability.
// A durability of 1.0f will eventually mean
// 1 second with the player's normal break speed.
    durability = 1.0f;


    // Most blocks do not spawn Jebubs.
    spawnsJebub = false;
    // Start every block's spawn timer at zero.
    //
    // Normal blocks never use this.
    // Spawner blocks count toward 60 seconds.
    jebubSpawnTimer = 0.0f;


    // --------------------------------------------------------
    // Block-specific properties
    // --------------------------------------------------------

    if (type == BlockType::Grass)
    {
        // Grass uses row 0 of the texture atlas.
        textureRow = 0;

        durability = 3.0f;
    }
    else if (type == BlockType::Spawner)
    {
        // Spawner uses row 1 of the texture atlas.
        textureRow = 1;

        // Spawners are the only current blocks
        // that are allowed to create Jebubs.
        spawnsJebub = true;

        durability = 999.0f;
    }
    else if (type == BlockType::Dirt)
    {
        // Dirt uses row 2 of the texture atlas.
        textureRow = 2;

        durability = 3.0f;
    }
    else if (type == BlockType::Wood)
    {
        // Wood uses row 3 of the texture atlas.
        textureRow = 3;

        durability = 5.0f;
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

        durability = 20.0f;
    }
}

// ============================================================
// Update block
//
// Handles special behavior that belongs to a block.
//
// Currently:
// - Spawner blocks create one Jebub every 60 seconds.
// ============================================================

void Block::update(
    float deltaTime,
    const glm::vec3& position,
    std::vector<NPC>& npcs
)
{
    // Normal blocks have nothing to update.
    if (!spawnsJebub)
    {
        return;
    }


    // Count time for this individual Spawner.
    jebubSpawnTimer +=
        deltaTime;


    // Wait until one minute has passed.
    if (jebubSpawnTimer < 60.0f)
    {
        return;
    }


    // Create a Jebub above this Spawner.
    npcs.emplace_back(
        NPCType::Jebub,
        glm::vec3(
            position.x,
            position.y + 1.15f,
            position.z
        )
    );


    // Start this Spawner's timer again.
    jebubSpawnTimer =
        0.0f;
}

bool Block::needsUpdate() const
{
    return spawnsJebub;
}