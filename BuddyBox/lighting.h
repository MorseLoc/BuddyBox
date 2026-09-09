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
// Stores lighting for every cell in one 16 x 16 x 16 chunk,
// including empty air.
//
// Light levels:
// 0  = dark
// 15 = fully lit
// ============================================================

struct LightChunk
{
    static const int CHUNK_SIZE = 16;

    static const int CELL_COUNT =
        CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE;

    std::array<std::uint8_t, CELL_COUNT> skyLight;
    std::array<std::uint8_t, CELL_COUNT> blockLight;

    // All cells start dark.
    LightChunk()
    {
        skyLight.fill(0);
        blockLight.fill(0);
    }
};


// ============================================================
// Lighting
//
// Stores light separately from world blocks so air can carry it.
// ============================================================

class Lighting
{
public:
    static const int MAX_LIGHT = 15;
    static const int CHUNK_SIZE = 16;


    // --------------------------------------------------------
    // Skylight
    // --------------------------------------------------------

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


    // --------------------------------------------------------
    // Block light
    //
    // Storage for light from future emissive blocks.
    // --------------------------------------------------------

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


    // --------------------------------------------------------
    // Combined light
    // --------------------------------------------------------

    // Return the brighter of skylight and block light.
    int getLight(
        int x,
        int y,
        int z
    ) const;


    // --------------------------------------------------------
    // Initial world lighting
    // --------------------------------------------------------

    // Calculate skylight for the loaded world.
    void calculateSkyLight(
        const World& world
    );


    // --------------------------------------------------------
    // Update lighting after a block changes
    // --------------------------------------------------------

    // Call AFTER placing or removing the block.
    //
    // Returns the chunks whose rendered lighting needs updating.
    std::set<std::tuple<int, int, int>> updateBlockChange(
        const World& world,
        int x,
        int y,
        int z
    );


    // --------------------------------------------------------
    // Clear stored lighting
    // --------------------------------------------------------

    void clear();


private:
    // --------------------------------------------------------
    // Solid blocks in each vertical column
    //
    // Key: X, Z
    // Value: all solid block heights in that column
    // --------------------------------------------------------

    std::map<
        std::pair<int, int>,
        std::set<int>
    > solidColumnHeights;


    // --------------------------------------------------------
    // Light chunk storage
    //
    // Key: chunk X, Y, Z
    // --------------------------------------------------------

    std::map<
        std::tuple<int, int, int>,
        LightChunk
    > lightChunks;


    // --------------------------------------------------------
    // Coordinate helpers
    // --------------------------------------------------------

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


    // --------------------------------------------------------
    // Chunk access
    // --------------------------------------------------------

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


    // --------------------------------------------------------
    // Extend lighting into newly built areas
    // --------------------------------------------------------

    // Initialize direct sunlight in new chunks before
    // spreading weaker light into their shaded cells.
    void extendSkyLightArea(
        const World& world,
        int x,
        int y,
        int z,
        std::set<std::tuple<int, int, int>>& dirtyChunks
    );


    // --------------------------------------------------------
    // Dynamic lighting helpers
    // --------------------------------------------------------

    // Record which chunk meshes sample a changed light cell.
    void addDirtyChunkForCell(
        std::set<std::tuple<int, int, int>>& dirtyChunks,
        int x,
        int y,
        int z
    ) const;

    // Spread newly available skylight.
    void propagateSkyLightAddition(
        const World& world,
        int startX,
        int startY,
        int startZ,
        std::set<std::tuple<int, int, int>>& dirtyChunks
    );

    // Remove invalid light, then restore surviving light paths.
    void propagateSkyLightRemoval(
        const World& world,
        int startX,
        int startY,
        int startZ,
        int oldLight,
        std::set<std::tuple<int, int, int>>& dirtyChunks
    );

    // Return 15 when the air cell has direct sky access,
    // otherwise return 0.
    int findDirectSkyLight(
        const World& world,
        int x,
        int y,
        int z
    ) const;

    // Rebuild light created by emissive blocks such as Bulbs.
    void calculateBlockLight(
        const World& world
    );

    // --------------------------------------------------------
    // Utility
    // --------------------------------------------------------

    // Keep light levels between 0 and 15.
    int clampLight(
        int lightLevel
    ) const;
};