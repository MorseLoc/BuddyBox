#include "lighting.h"

#include "World.h"

#include <algorithm>
#include <map>
#include <queue>
#include <set>
#include <tuple>
#include <utility>
#include <vector>


// ============================================================
// Small position type used by the propagation queue
// ============================================================

struct LightPosition
{
    int x;
    int y;
    int z;
};


// ============================================================
// Get chunk coordinate
//
// Converts a world coordinate into a chunk coordinate.
//
// This correctly handles negative coordinates.
// ============================================================

int Lighting::getChunkCoordinate(
    int worldCoordinate
) const
{
    if (worldCoordinate >= 0)
    {
        return
            worldCoordinate /
            CHUNK_SIZE;
    }


    return
        (
            worldCoordinate -
            (CHUNK_SIZE - 1)
            )
        /
        CHUNK_SIZE;
}


// ============================================================
// Get local coordinate
//
// Converts a world coordinate into a position from:
//
// 0 - 15
//
// inside its chunk.
// ============================================================

int Lighting::getLocalCoordinate(
    int worldCoordinate,
    int chunkCoordinate
) const
{
    return
        worldCoordinate -
        chunkCoordinate *
        CHUNK_SIZE;
}


// ============================================================
// Get cell index
//
// Converts:
//
// localX
// localY
// localZ
//
// into one index inside the 4096-cell arrays.
// ============================================================

int Lighting::getCellIndex(
    int localX,
    int localY,
    int localZ
) const
{
    return
        localX +
        (
            localZ *
            CHUNK_SIZE
            ) +
        (
            localY *
            CHUNK_SIZE *
            CHUNK_SIZE
            );
}


// ============================================================
// Clamp light
// ============================================================

int Lighting::clampLight(
    int lightLevel
) const
{
    if (lightLevel < 0)
    {
        return 0;
    }


    if (lightLevel > MAX_LIGHT)
    {
        return MAX_LIGHT;
    }


    return lightLevel;
}


// ============================================================
// Get or create light chunk
// ============================================================

LightChunk& Lighting::getOrCreateLightChunk(
    int chunkX,
    int chunkY,
    int chunkZ
)
{
    return
        lightChunks[
            std::make_tuple(
                chunkX,
                chunkY,
                chunkZ
            )
        ];
}


// ============================================================
// Find light chunk
//
// Does NOT create a chunk.
//
// Returns nullptr when the chunk has no stored lighting.
// ============================================================

const LightChunk* Lighting::findLightChunk(
    int chunkX,
    int chunkY,
    int chunkZ
) const
{
    auto iterator =
        lightChunks.find(
            std::make_tuple(
                chunkX,
                chunkY,
                chunkZ
            )
        );


    if (
        iterator ==
        lightChunks.end()
        )
    {
        return nullptr;
    }


    return
        &iterator->second;
}


// ============================================================
// Set sky light
// ============================================================

void Lighting::setSkyLight(
    int x,
    int y,
    int z,
    int lightLevel
)
{
    int chunkX =
        getChunkCoordinate(x);

    int chunkY =
        getChunkCoordinate(y);

    int chunkZ =
        getChunkCoordinate(z);


    int localX =
        getLocalCoordinate(
            x,
            chunkX
        );

    int localY =
        getLocalCoordinate(
            y,
            chunkY
        );

    int localZ =
        getLocalCoordinate(
            z,
            chunkZ
        );


    int index =
        getCellIndex(
            localX,
            localY,
            localZ
        );


    LightChunk& lightChunk =
        getOrCreateLightChunk(
            chunkX,
            chunkY,
            chunkZ
        );


    lightChunk.skyLight[index] =
        static_cast<std::uint8_t>(
            clampLight(
                lightLevel
            )
            );
}


// ============================================================
// Get sky light
// ============================================================

int Lighting::getSkyLight(
    int x,
    int y,
    int z
) const
{
    int chunkX =
        getChunkCoordinate(x);

    int chunkY =
        getChunkCoordinate(y);

    int chunkZ =
        getChunkCoordinate(z);


    const LightChunk* lightChunk =
        findLightChunk(
            chunkX,
            chunkY,
            chunkZ
        );


    if (lightChunk == nullptr)
    {
        return 0;
    }


    int localX =
        getLocalCoordinate(
            x,
            chunkX
        );

    int localY =
        getLocalCoordinate(
            y,
            chunkY
        );

    int localZ =
        getLocalCoordinate(
            z,
            chunkZ
        );


    int index =
        getCellIndex(
            localX,
            localY,
            localZ
        );


    return
        static_cast<int>(
            lightChunk->skyLight[index]
            );
}


// ============================================================
// Set block light
//
// This channel will be used by torches,
// glowing blocks, furnaces, etc.
// ============================================================

void Lighting::setBlockLight(
    int x,
    int y,
    int z,
    int lightLevel
)
{
    int chunkX =
        getChunkCoordinate(x);

    int chunkY =
        getChunkCoordinate(y);

    int chunkZ =
        getChunkCoordinate(z);


    int localX =
        getLocalCoordinate(
            x,
            chunkX
        );

    int localY =
        getLocalCoordinate(
            y,
            chunkY
        );

    int localZ =
        getLocalCoordinate(
            z,
            chunkZ
        );


    int index =
        getCellIndex(
            localX,
            localY,
            localZ
        );


    LightChunk& lightChunk =
        getOrCreateLightChunk(
            chunkX,
            chunkY,
            chunkZ
        );


    lightChunk.blockLight[index] =
        static_cast<std::uint8_t>(
            clampLight(
                lightLevel
            )
            );
}


// ============================================================
// Get block light
// ============================================================

int Lighting::getBlockLight(
    int x,
    int y,
    int z
) const
{
    int chunkX =
        getChunkCoordinate(x);

    int chunkY =
        getChunkCoordinate(y);

    int chunkZ =
        getChunkCoordinate(z);


    const LightChunk* lightChunk =
        findLightChunk(
            chunkX,
            chunkY,
            chunkZ
        );


    if (lightChunk == nullptr)
    {
        return 0;
    }


    int localX =
        getLocalCoordinate(
            x,
            chunkX
        );

    int localY =
        getLocalCoordinate(
            y,
            chunkY
        );

    int localZ =
        getLocalCoordinate(
            z,
            chunkZ
        );


    int index =
        getCellIndex(
            localX,
            localY,
            localZ
        );


    return
        static_cast<int>(
            lightChunk->blockLight[index]
            );
}


// ============================================================
// Get combined light
// ============================================================

int Lighting::getLight(
    int x,
    int y,
    int z
) const
{
    return
        std::max(
            getSkyLight(
                x,
                y,
                z
            ),
            getBlockLight(
                x,
                y,
                z
            )
        );
}


// ============================================================
// Clear
// ============================================================

void Lighting::clear()
{
    lightChunks.clear();

    solidColumnHeights.clear();
}


// ============================================================
// Calculate sky light
//
// Creates the initial lighting state for a loaded world.
//
// IMPORTANT:
//
// Light is stored in AIR CELLS.
//
// Direct sky:
//
// 15
// 15
// 15
// ██ block
//
// Light entering underneath roofs / caves:
//
// 15 -> 14 -> 13 -> 12 -> ...
//
// We operate on chunks containing world blocks plus a
// one-chunk border.
//
// One chunk is 16 cells wide and light only travels
// a maximum of 15 cells, so this border contains every
// cell that the current world geometry can illuminate.
// ============================================================

void Lighting::calculateSkyLight(
    const World& world
)
{
    clear();


    if (world.blocks.empty())
    {
        return;
    }


    // ========================================================
    // 1. Find the highest solid block in each X/Z column
    // ========================================================

    std::map<
        std::pair<int, int>,
        int
    > highestSolidBlock;


    // ========================================================
    // 2. Find chunks containing world geometry
    // ========================================================

    std::set<
        std::tuple<int, int, int>
    > worldChunks;


    for (const auto& entry : world.blocks)
    {
        int x =
            std::get<0>(
                entry.first
            );

        int y =
            std::get<1>(
                entry.first
            );

        int z =
            std::get<2>(
                entry.first
            );


        int chunkX =
            getChunkCoordinate(x);

        int chunkY =
            getChunkCoordinate(y);

        int chunkZ =
            getChunkCoordinate(z);


        worldChunks.insert(
            std::make_tuple(
                chunkX,
                chunkY,
                chunkZ
            )
        );


        // ----------------------------------------------------
        // Only solid blocks stop direct sunlight.
        // ----------------------------------------------------

        if (!entry.second.solid)
        {
            continue;
        }

        solidColumnHeights[
            std::make_pair(
                x,
                z
            )
        ].insert(
            y
        );

        std::pair<int, int> column =
            std::make_pair(
                x,
                z
            );


        auto highestIterator =
            highestSolidBlock.find(
                column
            );


        if (
            highestIterator ==
            highestSolidBlock.end()
            )
        {
            highestSolidBlock[column] =
                y;
        }
        else if (
            y >
            highestIterator->second
            )
        {
            highestIterator->second =
                y;
        }
    }


    // ========================================================
    // 3. Build the set of chunks that lighting may use
    //
    // Light can travel at most 15 cells.
    //
    // Since one chunk is 16 cells wide,
    // one surrounding chunk is enough.
    // ========================================================

    std::set<
        std::tuple<int, int, int>
    > lightingChunks;


    for (const auto& chunkPosition : worldChunks)
    {
        int chunkX =
            std::get<0>(
                chunkPosition
            );

        int chunkY =
            std::get<1>(
                chunkPosition
            );

        int chunkZ =
            std::get<2>(
                chunkPosition
            );


        for (
            int offsetX = -1;
            offsetX <= 1;
            offsetX++
            )
        {
            for (
                int offsetY = -1;
                offsetY <= 1;
                offsetY++
                )
            {
                for (
                    int offsetZ = -1;
                    offsetZ <= 1;
                    offsetZ++
                    )
                {
                    lightingChunks.insert(
                        std::make_tuple(
                            chunkX + offsetX,
                            chunkY + offsetY,
                            chunkZ + offsetZ
                        )
                    );
                }
            }
        }
    }


    // --------------------------------------------------------
    // Helper:
    //
    // Returns true when a world position belongs to one of
    // the chunks currently participating in lighting.
    // --------------------------------------------------------

    auto isInsideLightingArea =
        [&](int x, int y, int z)
        {
            int chunkX =
                getChunkCoordinate(x);

            int chunkY =
                getChunkCoordinate(y);

            int chunkZ =
                getChunkCoordinate(z);


            return
                lightingChunks.find(
                    std::make_tuple(
                        chunkX,
                        chunkY,
                        chunkZ
                    )
                )
                !=
                lightingChunks.end();
        };


    // ========================================================
    // 4. Seed direct skylight
    //
    // Every empty cell with no solid block above it
    // receives sky light 15.
    // ========================================================

    for (const auto& chunkPosition : lightingChunks)
    {
        int chunkX =
            std::get<0>(
                chunkPosition
            );

        int chunkY =
            std::get<1>(
                chunkPosition
            );

        int chunkZ =
            std::get<2>(
                chunkPosition
            );


        int startX =
            chunkX *
            CHUNK_SIZE;

        int startY =
            chunkY *
            CHUNK_SIZE;

        int startZ =
            chunkZ *
            CHUNK_SIZE;


        for (
            int localX = 0;
            localX < CHUNK_SIZE;
            localX++
            )
        {
            int x =
                startX +
                localX;


            for (
                int localZ = 0;
                localZ < CHUNK_SIZE;
                localZ++
                )
            {
                int z =
                    startZ +
                    localZ;


                std::pair<int, int> column =
                    std::make_pair(
                        x,
                        z
                    );


                auto highestIterator =
                    highestSolidBlock.find(
                        column
                    );


                bool columnHasSolidBlock =
                    highestIterator !=
                    highestSolidBlock.end();


                int highestY =
                    0;


                if (columnHasSolidBlock)
                {
                    highestY =
                        highestIterator->second;
                }


                for (
                    int localY = 0;
                    localY < CHUNK_SIZE;
                    localY++
                    )
                {
                    int y =
                        startY +
                        localY;


                    // Solid cells do not carry skylight.

                    if (
                        world.isSolidAt(
                            x,
                            y,
                            z
                        )
                        )
                    {
                        continue;
                    }


                    // No solid block exists anywhere
                    // above or below in this column.

                    if (!columnHasSolidBlock)
                    {
                        setSkyLight(
                            x,
                            y,
                            z,
                            MAX_LIGHT
                        );

                        continue;
                    }


                    // This air cell is above the highest
                    // solid block and therefore sees sky.

                    if (y > highestY)
                    {
                        setSkyLight(
                            x,
                            y,
                            z,
                            MAX_LIGHT
                        );
                    }
                }
            }
        }
    }


    // ========================================================
    // 5. Find skylight propagation frontiers
    //
    // We do NOT put every sky-lit cell into the queue.
    //
    // Only full-light cells next to a darker air cell need
    // to begin propagation.
    //
    // This keeps the queue much smaller.
    // ========================================================

    std::queue<
        LightPosition
    > propagationQueue;


    const int neighborOffsets[6][3] =
    {
        {  1,  0,  0 },
        { -1,  0,  0 },
        {  0,  1,  0 },
        {  0, -1,  0 },
        {  0,  0,  1 },
        {  0,  0, -1 }
    };


    for (const auto& chunkPosition : lightingChunks)
    {
        int chunkX =
            std::get<0>(
                chunkPosition
            );

        int chunkY =
            std::get<1>(
                chunkPosition
            );

        int chunkZ =
            std::get<2>(
                chunkPosition
            );


        int startX =
            chunkX *
            CHUNK_SIZE;

        int startY =
            chunkY *
            CHUNK_SIZE;

        int startZ =
            chunkZ *
            CHUNK_SIZE;


        for (
            int localX = 0;
            localX < CHUNK_SIZE;
            localX++
            )
        {
            for (
                int localY = 0;
                localY < CHUNK_SIZE;
                localY++
                )
            {
                for (
                    int localZ = 0;
                    localZ < CHUNK_SIZE;
                    localZ++
                    )
                {
                    int x =
                        startX +
                        localX;

                    int y =
                        startY +
                        localY;

                    int z =
                        startZ +
                        localZ;


                    if (
                        getSkyLight(
                            x,
                            y,
                            z
                        )
                        !=
                        MAX_LIGHT
                        )
                    {
                        continue;
                    }


                    bool touchesDarkAir =
                        false;


                    for (
                        int neighbor = 0;
                        neighbor < 6;
                        neighbor++
                        )
                    {
                        int neighborX =
                            x +
                            neighborOffsets[neighbor][0];

                        int neighborY =
                            y +
                            neighborOffsets[neighbor][1];

                        int neighborZ =
                            z +
                            neighborOffsets[neighbor][2];


                        if (
                            !isInsideLightingArea(
                                neighborX,
                                neighborY,
                                neighborZ
                            )
                            )
                        {
                            continue;
                        }


                        if (
                            world.isSolidAt(
                                neighborX,
                                neighborY,
                                neighborZ
                            )
                            )
                        {
                            continue;
                        }


                        if (
                            getSkyLight(
                                neighborX,
                                neighborY,
                                neighborZ
                            )
                            <
                            MAX_LIGHT - 1
                            )
                        {
                            touchesDarkAir =
                                true;

                            break;
                        }
                    }


                    if (touchesDarkAir)
                    {
                        propagationQueue.push(
                            {
                                x,
                                y,
                                z
                            }
                        );
                    }
                }
            }
        }
    }


    // ========================================================
    // 6. Propagate skylight
    //
    // Example:
    //
    // 15 -> 14 -> 13 -> 12 -> ...
    //
    // Solid blocks stop propagation.
    // ========================================================

    while (!propagationQueue.empty())
    {
        LightPosition current =
            propagationQueue.front();

        propagationQueue.pop();


        int currentLight =
            getSkyLight(
                current.x,
                current.y,
                current.z
            );


        if (currentLight <= 1)
        {
            continue;
        }


        int nextLight =
            currentLight - 1;


        for (
            int neighbor = 0;
            neighbor < 6;
            neighbor++
            )
        {
            int neighborX =
                current.x +
                neighborOffsets[neighbor][0];

            int neighborY =
                current.y +
                neighborOffsets[neighbor][1];

            int neighborZ =
                current.z +
                neighborOffsets[neighbor][2];


            // Do not create lighting endlessly
            // outside currently loaded world chunks.

            if (
                !isInsideLightingArea(
                    neighborX,
                    neighborY,
                    neighborZ
                )
                )
            {
                continue;
            }


            // Light cannot travel through solid blocks.

            if (
                world.isSolidAt(
                    neighborX,
                    neighborY,
                    neighborZ
                )
                )
            {
                continue;
            }


            int existingLight =
                getSkyLight(
                    neighborX,
                    neighborY,
                    neighborZ
                );


            if (
                existingLight >=
                nextLight
                )
            {
                continue;
            }


            setSkyLight(
                neighborX,
                neighborY,
                neighborZ,
                nextLight
            );


            propagationQueue.push(
                {
                    neighborX,
                    neighborY,
                    neighborZ
                }
            );
        }
    }
}

// ============================================================
// Add dirty chunk for light cell
//
// A lighting cell can affect a block face in its own chunk.
//
// If the cell sits on a chunk boundary, a face belonging to
// the neighboring chunk may also be sampling this light cell.
// ============================================================

void Lighting::addDirtyChunkForCell(
    std::set<
    std::tuple<int, int, int>
    >& dirtyChunks,
    int x,
    int y,
    int z
) const
{
    int chunkX =
        getChunkCoordinate(
            x
        );

    int chunkY =
        getChunkCoordinate(
            y
        );

    int chunkZ =
        getChunkCoordinate(
            z
        );


    int localX =
        getLocalCoordinate(
            x,
            chunkX
        );

    int localY =
        getLocalCoordinate(
            y,
            chunkY
        );

    int localZ =
        getLocalCoordinate(
            z,
            chunkZ
        );


    // --------------------------------------------------------
    // Main chunk
    // --------------------------------------------------------

    dirtyChunks.insert(
        std::make_tuple(
            chunkX,
            chunkY,
            chunkZ
        )
    );


    // --------------------------------------------------------
    // Neighboring chunks
    //
    // Only needed when this light cell touches a boundary.
    // --------------------------------------------------------

    if (localX == 0)
    {
        dirtyChunks.insert(
            std::make_tuple(
                chunkX - 1,
                chunkY,
                chunkZ
            )
        );
    }


    if (
        localX ==
        CHUNK_SIZE - 1
        )
    {
        dirtyChunks.insert(
            std::make_tuple(
                chunkX + 1,
                chunkY,
                chunkZ
            )
        );
    }


    if (localY == 0)
    {
        dirtyChunks.insert(
            std::make_tuple(
                chunkX,
                chunkY - 1,
                chunkZ
            )
        );
    }


    if (
        localY ==
        CHUNK_SIZE - 1
        )
    {
        dirtyChunks.insert(
            std::make_tuple(
                chunkX,
                chunkY + 1,
                chunkZ
            )
        );
    }


    if (localZ == 0)
    {
        dirtyChunks.insert(
            std::make_tuple(
                chunkX,
                chunkY,
                chunkZ - 1
            )
        );
    }


    if (
        localZ ==
        CHUNK_SIZE - 1
        )
    {
        dirtyChunks.insert(
            std::make_tuple(
                chunkX,
                chunkY,
                chunkZ + 1
            )
        );
    }
}


// ============================================================
// Find direct sky light
//
// Returns:
//
// 15
//     air cell has nothing solid above it
//
// 0
//     air cell is underneath a solid block
//     or the cell itself is solid
// ============================================================

int Lighting::findDirectSkyLight(
    const World& world,
    int x,
    int y,
    int z
) const
{
    // Solid cells cannot contain light.

    if (
        world.isSolidAt(
            x,
            y,
            z
        )
        )
    {
        return 0;
    }


    std::pair<int, int> column =
        std::make_pair(
            x,
            z
        );


    auto columnIterator =
        solidColumnHeights.find(
            column
        );


    // No solid blocks exist anywhere in this column.

    if (
        columnIterator ==
        solidColumnHeights.end()
        )
    {
        return MAX_LIGHT;
    }


    if (
        columnIterator->second.empty()
        )
    {
        return MAX_LIGHT;
    }


    // Highest solid Y in the column.

    int highestSolidY =
        *columnIterator->second.rbegin();


    if (y > highestSolidY)
    {
        return MAX_LIGHT;
    }


    return 0;
}


// ============================================================
// Propagate sky light addition
//
// Used when new air appears or new sunlight enters.
//
// Light spreads:
//
// 15 -> 14 -> 13 -> 12 ...
// ============================================================

void Lighting::propagateSkyLightAddition(
    const World& world,
    int startX,
    int startY,
    int startZ,
    std::set<
    std::tuple<int, int, int>
    >& dirtyChunks
)
{
    if (
        world.isSolidAt(
            startX,
            startY,
            startZ
        )
        )
    {
        return;
    }


    struct LightNode
    {
        int x;
        int y;
        int z;
    };


    const int neighborOffsets[6][3] =
    {
        {  1,  0,  0 },
        { -1,  0,  0 },
        {  0,  1,  0 },
        {  0, -1,  0 },
        {  0,  0,  1 },
        {  0,  0, -1 }
    };


    // --------------------------------------------------------
    // Find how bright the starting cell should be.
    // --------------------------------------------------------

    int bestLight =
        findDirectSkyLight(
            world,
            startX,
            startY,
            startZ
        );


    for (
        int neighbor = 0;
        neighbor < 6;
        neighbor++
        )
    {
        int neighborX =
            startX +
            neighborOffsets[neighbor][0];

        int neighborY =
            startY +
            neighborOffsets[neighbor][1];

        int neighborZ =
            startZ +
            neighborOffsets[neighbor][2];


        if (
            world.isSolidAt(
                neighborX,
                neighborY,
                neighborZ
            )
            )
        {
            continue;
        }


        int neighborLight =
            getSkyLight(
                neighborX,
                neighborY,
                neighborZ
            );


        int possibleLight =
            neighborLight - 1;


        if (
            possibleLight >
            bestLight
            )
        {
            bestLight =
                possibleLight;
        }
    }


    bestLight =
        clampLight(
            bestLight
        );


    int existingStartLight =
        getSkyLight(
            startX,
            startY,
            startZ
        );


    if (
        bestLight >
        existingStartLight
        )
    {
        setSkyLight(
            startX,
            startY,
            startZ,
            bestLight
        );


        addDirtyChunkForCell(
            dirtyChunks,
            startX,
            startY,
            startZ
        );
    }


    if (bestLight <= 1)
    {
        return;
    }


    // ========================================================
    // Breadth-first light propagation
    // ========================================================

    std::queue<
        LightNode
    > propagationQueue;


    propagationQueue.push(
        {
            startX,
            startY,
            startZ
        }
    );


    while (!propagationQueue.empty())
    {
        LightNode current =
            propagationQueue.front();

        propagationQueue.pop();


        int currentLight =
            getSkyLight(
                current.x,
                current.y,
                current.z
            );


        if (currentLight <= 1)
        {
            continue;
        }


        int nextLight =
            currentLight - 1;


        for (
            int neighbor = 0;
            neighbor < 6;
            neighbor++
            )
        {
            int neighborX =
                current.x +
                neighborOffsets[neighbor][0];

            int neighborY =
                current.y +
                neighborOffsets[neighbor][1];

            int neighborZ =
                current.z +
                neighborOffsets[neighbor][2];


            if (
                world.isSolidAt(
                    neighborX,
                    neighborY,
                    neighborZ
                )
                )
            {
                continue;
            }


            int existingLight =
                getSkyLight(
                    neighborX,
                    neighborY,
                    neighborZ
                );


            if (
                existingLight >=
                nextLight
                )
            {
                continue;
            }


            setSkyLight(
                neighborX,
                neighborY,
                neighborZ,
                nextLight
            );


            addDirtyChunkForCell(
                dirtyChunks,
                neighborX,
                neighborY,
                neighborZ
            );


            propagationQueue.push(
                {
                    neighborX,
                    neighborY,
                    neighborZ
                }
            );
        }
    }
}


// ============================================================
// Propagate sky light removal
//
// Removes light that depended on a light cell which
// has disappeared.
//
// Surviving light sources are remembered and propagated
// again afterward.
// ============================================================

void Lighting::propagateSkyLightRemoval(
    const World& world,
    int startX,
    int startY,
    int startZ,
    int oldLight,
    std::set<
    std::tuple<int, int, int>
    >& dirtyChunks
)
{
    if (oldLight <= 0)
    {
        return;
    }


    struct RemovalNode
    {
        int x;
        int y;
        int z;
        int oldLight;
    };


    const int neighborOffsets[6][3] =
    {
        {  1,  0,  0 },
        { -1,  0,  0 },
        {  0,  1,  0 },
        {  0, -1,  0 },
        {  0,  0,  1 },
        {  0,  0, -1 }
    };


    std::queue<
        RemovalNode
    > removalQueue;


    std::set<
        std::tuple<int, int, int>
    > relightCells;


    removalQueue.push(
        {
            startX,
            startY,
            startZ,
            oldLight
        }
    );


    while (!removalQueue.empty())
    {
        RemovalNode current =
            removalQueue.front();

        removalQueue.pop();


        for (
            int neighbor = 0;
            neighbor < 6;
            neighbor++
            )
        {
            int neighborX =
                current.x +
                neighborOffsets[neighbor][0];

            int neighborY =
                current.y +
                neighborOffsets[neighbor][1];

            int neighborZ =
                current.z +
                neighborOffsets[neighbor][2];


            if (
                world.isSolidAt(
                    neighborX,
                    neighborY,
                    neighborZ
                )
                )
            {
                continue;
            }


            int neighborLight =
                getSkyLight(
                    neighborX,
                    neighborY,
                    neighborZ
                );


            if (neighborLight <= 0)
            {
                continue;
            }


            // ------------------------------------------------
            // Direct sunlight is an independent source.
            //
            // Never remove it because another light path
            // disappeared.
            // ------------------------------------------------

            if (
                findDirectSkyLight(
                    world,
                    neighborX,
                    neighborY,
                    neighborZ
                )
                ==
                MAX_LIGHT
                )
            {
                relightCells.insert(
                    std::make_tuple(
                        neighborX,
                        neighborY,
                        neighborZ
                    )
                );

                continue;
            }


            // ------------------------------------------------
            // A lower light value depended on the brighter
            // cell that was removed.
            // ------------------------------------------------

            if (
                neighborLight <
                current.oldLight
                )
            {
                setSkyLight(
                    neighborX,
                    neighborY,
                    neighborZ,
                    0
                );


                addDirtyChunkForCell(
                    dirtyChunks,
                    neighborX,
                    neighborY,
                    neighborZ
                );


                removalQueue.push(
                    {
                        neighborX,
                        neighborY,
                        neighborZ,
                        neighborLight
                    }
                );
            }
            else
            {
                // Equal or brighter light survived from
                // another direction.

                relightCells.insert(
                    std::make_tuple(
                        neighborX,
                        neighborY,
                        neighborZ
                    )
                );
            }
        }
    }


    // ========================================================
    // Re-spread surviving nearby light.
    // ========================================================

    for (const auto& position : relightCells)
    {
        propagateSkyLightAddition(
            world,
            std::get<0>(
                position
            ),
            std::get<1>(
                position
            ),
            std::get<2>(
                position
            ),
            dirtyChunks
        );
    }
}


// ============================================================
// Update block change
//
// Call AFTER World has placed or removed the block.
//
// This updates:
//
// - direct skylight
// - propagated skylight
// - removed skylight
// - dirty render chunks
//
// It does NOT recalculate the entire world.
// ============================================================

std::set<
    std::tuple<int, int, int>
> Lighting::updateBlockChange(
    const World& world,
    int x,
    int y,
    int z
)
{
    std::set<
        std::tuple<int, int, int>
    > dirtyChunks;


    std::pair<int, int> column =
        std::make_pair(
            x,
            z
        );


    // ========================================================
    // What did this cell look like BEFORE the world edit?
    //
    // The column cache still contains the previous state.
    // ========================================================

    auto oldColumnIterator =
        solidColumnHeights.find(
            column
        );


    bool wasSolid =
        false;


    bool hadOldHighest =
        false;


    int oldHighest =
        0;


    if (
        oldColumnIterator !=
        solidColumnHeights.end()
        )
    {
        wasSolid =
            oldColumnIterator->second.find(
                y
            )
            !=
            oldColumnIterator->second.end();


        if (
            !oldColumnIterator->second.empty()
            )
        {
            hadOldHighest =
                true;

            oldHighest =
                *oldColumnIterator->second.rbegin();
        }
    }


    int changedCellOldLight =
        getSkyLight(
            x,
            y,
            z
        );


    // ========================================================
    // Update our column cache to match the NEW World state.
    // ========================================================

    bool isSolidNow =
        world.isSolidAt(
            x,
            y,
            z
        );


    if (isSolidNow)
    {
        solidColumnHeights[
            column
        ].insert(
            y
        );
    }
    else
    {
        auto columnIterator =
            solidColumnHeights.find(
                column
            );


        if (
            columnIterator !=
            solidColumnHeights.end()
            )
        {
            columnIterator->second.erase(
                y
            );


            if (
                columnIterator->second.empty()
                )
            {
                solidColumnHeights.erase(
                    columnIterator
                );
            }
        }
    }


    // ========================================================
    // Find new highest solid block.
    // ========================================================

    auto newColumnIterator =
        solidColumnHeights.find(
            column
        );


    bool hadNewHighest =
        false;


    int newHighest =
        0;


    if (
        newColumnIterator !=
        solidColumnHeights.end() &&
        !newColumnIterator->second.empty()
        )
    {
        hadNewHighest =
            true;

        newHighest =
            *newColumnIterator->second.rbegin();
    }


    // ========================================================
    // Direct sunlight may have changed for multiple cells in
    // this X/Z column.
    //
    // We inspect cells already participating in lighting.
    // ========================================================

    std::vector<
        int
    > columnCells;


    int columnChunkX =
        getChunkCoordinate(
            x
        );

    int columnChunkZ =
        getChunkCoordinate(
            z
        );


    int localX =
        getLocalCoordinate(
            x,
            columnChunkX
        );

    int localZ =
        getLocalCoordinate(
            z,
            columnChunkZ
        );


    for (const auto& chunkEntry : lightChunks)
    {
        int lightChunkX =
            std::get<0>(
                chunkEntry.first
            );

        int lightChunkY =
            std::get<1>(
                chunkEntry.first
            );

        int lightChunkZ =
            std::get<2>(
                chunkEntry.first
            );


        if (
            lightChunkX !=
            columnChunkX ||
            lightChunkZ !=
            columnChunkZ
            )
        {
            continue;
        }


        for (
            int localY = 0;
            localY < CHUNK_SIZE;
            localY++
            )
        {
            int worldY =
                lightChunkY *
                CHUNK_SIZE +
                localY;


            columnCells.push_back(
                worldY
            );
        }
    }


    std::map<
        std::tuple<int, int, int>,
        int
    > removalSeeds;


    std::set<
        std::tuple<int, int, int>
    > additionSeeds;


    for (int cellY : columnCells)
    {
        // ----------------------------------------------------
        // Was this cell solid before the edit?
        // ----------------------------------------------------

        bool oldCellSolid;


        if (cellY == y)
        {
            oldCellSolid =
                wasSolid;
        }
        else
        {
            oldCellSolid =
                world.isSolidAt(
                    x,
                    cellY,
                    z
                );
        }


        bool oldDirectSky =
            !oldCellSolid &&
            (
                !hadOldHighest ||
                cellY > oldHighest
                );


        bool newDirectSky =
            findDirectSkyLight(
                world,
                x,
                cellY,
                z
            )
            ==
            MAX_LIGHT;


        // ----------------------------------------------------
        // Direct sunlight disappeared.
        // ----------------------------------------------------

        if (
            oldDirectSky &&
            !newDirectSky
            )
        {
            int oldLight =
                getSkyLight(
                    x,
                    cellY,
                    z
                );


            if (oldLight > 0)
            {
                setSkyLight(
                    x,
                    cellY,
                    z,
                    0
                );


                addDirtyChunkForCell(
                    dirtyChunks,
                    x,
                    cellY,
                    z
                );


                removalSeeds[
                    std::make_tuple(
                        x,
                        cellY,
                        z
                    )
                ] =
                    oldLight;
            }
        }


        // ----------------------------------------------------
        // Direct sunlight appeared.
        // ----------------------------------------------------

        if (
            !oldDirectSky &&
            newDirectSky
            )
        {
            int existingLight =
                getSkyLight(
                    x,
                    cellY,
                    z
                );


            if (
                existingLight <
                MAX_LIGHT
                )
            {
                setSkyLight(
                    x,
                    cellY,
                    z,
                    MAX_LIGHT
                );


                addDirtyChunkForCell(
                    dirtyChunks,
                    x,
                    cellY,
                    z
                );
            }


            additionSeeds.insert(
                std::make_tuple(
                    x,
                    cellY,
                    z
                )
            );
        }
    }


    // ========================================================
    // Block was placed.
    //
    // The cell can no longer contain light.
    // ========================================================

    if (
        !wasSolid &&
        isSolidNow
        )
    {
        if (changedCellOldLight > 0)
        {
            setSkyLight(
                x,
                y,
                z,
                0
            );


            addDirtyChunkForCell(
                dirtyChunks,
                x,
                y,
                z
            );


            removalSeeds[
                std::make_tuple(
                    x,
                    y,
                    z
                )
            ] =
                changedCellOldLight;
        }
    }


    // ========================================================
    // Process light removal.
    // ========================================================

    for (const auto& removal : removalSeeds)
    {
        propagateSkyLightRemoval(
            world,
            std::get<0>(
                removal.first
            ),
            std::get<1>(
                removal.first
            ),
            std::get<2>(
                removal.first
            ),
            removal.second,
            dirtyChunks
        );
    }


    // ========================================================
    // Block was removed.
    //
    // The newly created air cell can now receive light from:
    //
    // - direct sky
    // - neighboring propagated skylight
    // ========================================================

    if (
        wasSolid &&
        !isSolidNow
        )
    {
        additionSeeds.insert(
            std::make_tuple(
                x,
                y,
                z
            )
        );
    }


    // ========================================================
    // Process new / surviving light.
    // ========================================================

    for (const auto& addition : additionSeeds)
    {
        propagateSkyLightAddition(
            world,
            std::get<0>(
                addition
            ),
            std::get<1>(
                addition
            ),
            std::get<2>(
                addition
            ),
            dirtyChunks
        );
    }


    return dirtyChunks;
}