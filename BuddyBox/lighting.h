#pragma once

#include <array>
#include <cstdint>
#include <map>
#include <tuple>
#include <set>
#include <utility>


struct World;


// ============================================================
// LightChunk
//
// Stores lighting for one 16 x 16 x 16 world chunk.
//
// Lighting exists for EVERY world cell,
// including empty air.
//
// Each cell stores:
//
// skyLight
// blockLight
//
// Values:
// 0  = dark
// 15 = fully lit
// ============================================================

struct LightChunk
{
    static const int CHUNK_SIZE =
        16;

    static const int CELL_COUNT =
        CHUNK_SIZE *
        CHUNK_SIZE *
        CHUNK_SIZE;


    std::array<
        std::uint8_t,
        CELL_COUNT
    > skyLight;


    std::array<
        std::uint8_t,
        CELL_COUNT
    > blockLight;


    // --------------------------------------------------------
    // Constructor
    //
    // All lighting starts at zero.
    // --------------------------------------------------------

    LightChunk()
    {
        skyLight.fill(
            0
        );

        blockLight.fill(
            0
        );
    }
};


// ============================================================
// Lighting
//
// Handles BuddyBox world lighting.
//
// Light is stored separately from Blocks.
//
// This means empty air can carry light,
// which is required for proper voxel lighting.
//
// The system stores:
//
// skyLight
//     Light originating from the sky.
//
// blockLight
//     Light originating from emissive blocks.
//
// Both use values:
//
// 0 - 15
// ============================================================

class Lighting
{
public:

    // --------------------------------------------------------
    // Light constants
    // --------------------------------------------------------

    static const int MAX_LIGHT =
        15;


    static const int CHUNK_SIZE =
        16;


    // ========================================================
    // Sky light
    // ========================================================

    void setSkyLight(
        int x,
        int y,
        int z,
        int lightLevel
    );


    int getSkyLight(
        int x,
        int y,
        int z
    ) const;


    // ========================================================
    // Block light
    // ========================================================

    void setBlockLight(
        int x,
        int y,
        int z,
        int lightLevel
    );


    int getBlockLight(
        int x,
        int y,
        int z
    ) const;


    // ========================================================
    // Combined light
    //
    // Useful when the renderer just needs the brightest
    // source at a world cell.
    // ========================================================

    int getLight(
        int x,
        int y,
        int z
    ) const;


    // ========================================================
    // Initial world lighting
    //
    // Builds the initial sky lighting for a loaded world.
    //
    // This will later:
    //
    // 1. Seed direct skylight
    // 2. Propagate skylight
    // 3. Seed emissive blocks
    // 4. Propagate block light
    // ========================================================

    void calculateSkyLight(
        const World& world
    );

    // ========================================================
// Dynamic world lighting
//
// Call this AFTER a block has been placed or removed.
//
// Updates only lighting affected by that world change.
//
// Returns the render chunks whose lighting changed,
// so BuddyBox only needs to rebuild those chunk meshes.
// ========================================================

    std::set<
        std::tuple<int, int, int>
    > updateBlockChange(
        const World& world,
        int x,
        int y,
        int z
    );


    // ========================================================
    // Clear lighting
    //
    // Intended mainly for loading/rebuilding a world,
    // not normal block edits.
    // ========================================================

    void clear();


private:

    // ========================================================
// Solid blocks in each X/Z column
//
// Used to determine which air cells have
// direct access to the sky.
//
// Key:
// x, z
//
// Value:
// all solid Y positions in that column
// ========================================================

    std::map<
        std::pair<int, int>,
        std::set<int>
    > solidColumnHeights;

    // ========================================================
    // Light chunk storage
    //
    // Key:
    // chunkX, chunkY, chunkZ
    //
    // Value:
    // LightChunk containing 4096 world cells.
    // ========================================================

    std::map<
        std::tuple<int, int, int>,
        LightChunk
    > lightChunks;


    // ========================================================
    // Coordinate helpers
    // ========================================================

    int getChunkCoordinate(
        int worldCoordinate
    ) const;


    int getLocalCoordinate(
        int worldCoordinate,
        int chunkCoordinate
    ) const;


    int getCellIndex(
        int localX,
        int localY,
        int localZ
    ) const;


    // ========================================================
    // Chunk access
    // ========================================================

    LightChunk& getOrCreateLightChunk(
        int chunkX,
        int chunkY,
        int chunkZ
    );


    const LightChunk* findLightChunk(
        int chunkX,
        int chunkY,
        int chunkZ
    ) const;

    // ========================================================
// Dynamic lighting helpers
// ========================================================

    void addDirtyChunkForCell(
        std::set<
        std::tuple<int, int, int>
        >& dirtyChunks,
        int x,
        int y,
        int z
    ) const;


    void propagateSkyLightAddition(
        const World& world,
        int startX,
        int startY,
        int startZ,
        std::set<
        std::tuple<int, int, int>
        >& dirtyChunks
    );


    void propagateSkyLightRemoval(
        const World& world,
        int startX,
        int startY,
        int startZ,
        int oldLight,
        std::set<
        std::tuple<int, int, int>
        >& dirtyChunks
    );


    int findDirectSkyLight(
        const World& world,
        int x,
        int y,
        int z
    ) const;


    // ========================================================
    // Utility
    // ========================================================

    int clampLight(
        int lightLevel
    ) const;
};