#pragma once

#include <glm/glm.hpp>
#include <vector>

class NPC;


// ============================================================
// Block types
//
// Lists every type of block that can currently exist
// in the BuddyBox world.
//
// Block.cpp decides the properties of each type.
// ============================================================

enum class BlockType
{
    Grass,
    Spawner,
    Dirt,
    Wood,
    Leaf,
    Stone
};


// ============================================================
// Block
//
// Represents one block in the world.
//
// Each Block stores:
// - What type of block it is
// - Its physical size
// - Whether it has collision
// - Whether it can spawn Jebubs
// - Its Jebub spawn timer
// - Which texture it uses
// ============================================================

struct Block
{
    // --------------------------------------------------------
    // Basic block properties
    // --------------------------------------------------------

    // What kind of block this is.
    BlockType type;


    // Physical size of the block.
    // Normal BuddyBox blocks are 1 x 1 x 1.
    glm::vec3 size;


    // true  = player cannot move through this block.
    // false = player can move through this block.
    bool solid;


    // --------------------------------------------------------
    // Spawner properties
    // --------------------------------------------------------

    // true if this block is allowed to create Jebubs.
    bool spawnsJebub;


    // Counts time since this block last spawned a Jebub.
    //
    // Only Spawner blocks actually use this timer.
    float jebubSpawnTimer;


    // --------------------------------------------------------
    // Texture
    // --------------------------------------------------------

    // Which row of the block texture atlas this block uses.
    //
    // Example:
    // 0 = Grass
    // 1 = Spawner
    // 2 = Dirt
    int textureRow;


    // --------------------------------------------------------
    // Block behavior
    // --------------------------------------------------------

    // Updates special behavior for this block.
    //
    // Spawner blocks use this to create Jebubs.
    void update(
        float deltaTime,
        const glm::vec3& position,
        std::vector<NPC>& npcs
    );


    // --------------------------------------------------------
    // Constructor
    // --------------------------------------------------------

    // Creates a block of the requested type.
    //
    // Block.cpp uses the type to assign the block's
    // size, collision, texture, and other properties.
    Block(BlockType type);

    bool needsUpdate() const;
};