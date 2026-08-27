#pragma once

#include <glm/glm.hpp>


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
// - Which texture it uses
// ============================================================

struct Block
{
    // What kind of block this is.
    BlockType type;


    // Physical size of the block.
    // Normal BuddyBox blocks are 1 x 1 x 1.
    glm::vec3 size;


    // true  = player cannot move through this block.
    // false = player can move through this block.
    bool solid;


    // true if this block is allowed to create Jebubs.
    bool spawnsJebub;


    // Which row of the block texture atlas this block uses.
    //
    // Example:
    // 0 = Grass
    // 1 = Spawner
    // 2 = Dirt
    int textureRow;


    // Creates a block of the requested type.
    //
    // Block.cpp uses the type to assign the block's
    // size, collision, texture, and other properties.
    Block(BlockType type);
};