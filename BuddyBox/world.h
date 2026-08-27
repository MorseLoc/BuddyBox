#pragma once

#include <map>
#include <string>
#include <tuple>

#include <glm/glm.hpp>

#include "Block.h"


// ============================================================
// World
//
// Represents the BuddyBox block world.
//
// The World stores blocks using integer grid coordinates.
//
// Example:
//
// (0, 0, 0) -> Grass
// (1, 0, 0) -> Stone
//
// World.cpp handles:
// - Block lookup
// - Collision queries
// - Placing and removing blocks
// - Block raycasting
// - Loading worlds from files
// ============================================================

struct World
{
    // --------------------------------------------------------
    // Block storage
    // --------------------------------------------------------

    // Every block is stored using its X, Y, Z grid position.
    //
    // The tuple is the coordinate.
    // The Block is the block stored there.
    std::map<
        std::tuple<int, int, int>,
        Block
    > blocks;


    // --------------------------------------------------------
    // Block lookup
    // --------------------------------------------------------

    // Returns true if ANY block exists
    // at this grid position.
    bool hasBlock(
        int x,
        int y,
        int z
    ) const;


    // Returns true if a SOLID block exists
    // at this grid position.
    //
    // Used by player collision.
    bool isSolidAt(
        int x,
        int y,
        int z
    ) const;


    // --------------------------------------------------------
    // Block editing
    // --------------------------------------------------------

    // Places a block at this grid position.
    //
    // If a block already exists there,
    // the existing block is replaced.
    void placeBlock(
        int x,
        int y,
        int z,
        const Block& block
    );


    // Removes the block at this grid position.
    void removeBlock(
        int x,
        int y,
        int z
    );


    // --------------------------------------------------------
    // Raycasting
    // --------------------------------------------------------

    // Shoots an invisible ray through the block grid
    // and returns the first block it touches.
    //
    // origin:
    //     Where the ray begins.
    //
    // direction:
    //     Which direction the ray travels.
    //
    // maxDistance:
    //     Maximum distance the ray can travel.
    //
    // hitX / hitY / hitZ:
    //     Coordinates of the block that was hit.
    //
    // previousX / previousY / previousZ:
    //     Grid position immediately before the hit block.
    //     Used when placing a new block beside it.
    //
    // Returns:
    // true  = a block was hit
    // false = nothing was hit
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


    // --------------------------------------------------------
    // World loading
    // --------------------------------------------------------

    // Loads blocks from a BuddyBox world text file.
    //
    // Returns:
    // true  = file opened successfully
    // false = file could not be opened
    bool loadFromFile(
        const std::string& filename
    );
};