#include "lighting.h"
#include "world.h"

#include <algorithm>
#include <map>
#include <queue>
#include <set>
#include <tuple>
#include <utility>
#include <vector>


// ============================================================
// Position used by the lighting queue
// ============================================================

struct LightPosition
{
    int x;
    int y;
    int z;
};


// ============================================================
// Coordinate helpers
// ============================================================

int Lighting::getChunkCoordinate(int worldCoordinate) const
{
    if (worldCoordinate >= 0)
    {
        return worldCoordinate / CHUNK_SIZE;
    }

    // Round negative coordinates down to the correct chunk.
    return (worldCoordinate - (CHUNK_SIZE - 1)) / CHUNK_SIZE;
}


int Lighting::getLocalCoordinate(
    int worldCoordinate,
    int chunkCoordinate
) const
{
    return worldCoordinate - chunkCoordinate * CHUNK_SIZE;
}


int Lighting::getCellIndex(
    int localX,
    int localY,
    int localZ
) const
{
    return localX
        + localZ * CHUNK_SIZE
        + localY * CHUNK_SIZE * CHUNK_SIZE;
}


int Lighting::clampLight(int lightLevel) const
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
// Light chunk access
// ============================================================

LightChunk& Lighting::getOrCreateLightChunk(
    int chunkX,
    int chunkY,
    int chunkZ
)
{
    return lightChunks[
        std::make_tuple(chunkX, chunkY, chunkZ)
    ];
}


const LightChunk* Lighting::findLightChunk(
    int chunkX,
    int chunkY,
    int chunkZ
) const
{
    auto iterator = lightChunks.find(
        std::make_tuple(chunkX, chunkY, chunkZ)
    );

    if (iterator == lightChunks.end())
    {
        return nullptr;
    }

    return &iterator->second;
}


// ============================================================
// Skylight storage
// ============================================================

void Lighting::setSkyLight(
    int x,
    int y,
    int z,
    int lightLevel
)
{
    int chunkX = getChunkCoordinate(x);
    int chunkY = getChunkCoordinate(y);
    int chunkZ = getChunkCoordinate(z);

    int localX = getLocalCoordinate(x, chunkX);
    int localY = getLocalCoordinate(y, chunkY);
    int localZ = getLocalCoordinate(z, chunkZ);

    int index = getCellIndex(localX, localY, localZ);

    LightChunk& lightChunk = getOrCreateLightChunk(
        chunkX, chunkY, chunkZ
    );

    lightChunk.skyLight[index] =
        static_cast<std::uint8_t>(clampLight(lightLevel));
}


int Lighting::getSkyLight(
    int x,
    int y,
    int z
) const
{
    int chunkX = getChunkCoordinate(x);
    int chunkY = getChunkCoordinate(y);
    int chunkZ = getChunkCoordinate(z);

    const LightChunk* lightChunk = findLightChunk(
        chunkX, chunkY, chunkZ
    );

    if (lightChunk == nullptr)
    {
        return 0;
    }

    int localX = getLocalCoordinate(x, chunkX);
    int localY = getLocalCoordinate(y, chunkY);
    int localZ = getLocalCoordinate(z, chunkZ);

    int index = getCellIndex(localX, localY, localZ);

    return static_cast<int>(lightChunk->skyLight[index]);
}


// ============================================================
// Block light storage
// ============================================================

void Lighting::setBlockLight(
    int x,
    int y,
    int z,
    int lightLevel
)
{
    int chunkX = getChunkCoordinate(x);
    int chunkY = getChunkCoordinate(y);
    int chunkZ = getChunkCoordinate(z);

    int localX = getLocalCoordinate(x, chunkX);
    int localY = getLocalCoordinate(y, chunkY);
    int localZ = getLocalCoordinate(z, chunkZ);

    int index = getCellIndex(localX, localY, localZ);

    LightChunk& lightChunk = getOrCreateLightChunk(
        chunkX, chunkY, chunkZ
    );

    lightChunk.blockLight[index] =
        static_cast<std::uint8_t>(clampLight(lightLevel));
}


int Lighting::getBlockLight(
    int x,
    int y,
    int z
) const
{
    int chunkX = getChunkCoordinate(x);
    int chunkY = getChunkCoordinate(y);
    int chunkZ = getChunkCoordinate(z);

    const LightChunk* lightChunk = findLightChunk(
        chunkX, chunkY, chunkZ
    );

    if (lightChunk == nullptr)
    {
        return 0;
    }

    int localX = getLocalCoordinate(x, chunkX);
    int localY = getLocalCoordinate(y, chunkY);
    int localZ = getLocalCoordinate(z, chunkZ);

    int index = getCellIndex(localX, localY, localZ);

    return static_cast<int>(lightChunk->blockLight[index]);
}


int Lighting::getLight(
    int x,
    int y,
    int z
) const
{
    return std::max(
        getSkyLight(x, y, z),
        getBlockLight(x, y, z)
    );
}


void Lighting::clear()
{
    lightChunks.clear();
    solidColumnHeights.clear();
}


// ============================================================
// Calculate the world's initial skylight
// ============================================================

void Lighting::calculateSkyLight(const World& world)
{
    clear();

    if (world.blocks.empty())
    {
        return;
    }

    std::map<std::pair<int, int>, int> highestSolidBlock;
    std::set<std::tuple<int, int, int>> worldChunks;

    // Find occupied chunks and solid block heights.
    for (const auto& entry : world.blocks)
    {
        int x = std::get<0>(entry.first);
        int y = std::get<1>(entry.first);
        int z = std::get<2>(entry.first);

        int chunkX = getChunkCoordinate(x);
        int chunkY = getChunkCoordinate(y);
        int chunkZ = getChunkCoordinate(z);

        worldChunks.insert(
            std::make_tuple(chunkX, chunkY, chunkZ)
        );

        if (!entry.second.solid)
        {
            continue;
        }

        solidColumnHeights[std::make_pair(x, z)].insert(y);

        std::pair<int, int> column = std::make_pair(x, z);
        auto highestIterator = highestSolidBlock.find(column);

        if (highestIterator == highestSolidBlock.end())
        {
            highestSolidBlock[column] = y;
        }
        else if (y > highestIterator->second)
        {
            highestIterator->second = y;
        }
    }

    // Include one chunk of surrounding air in every direction.
    std::set<std::tuple<int, int, int>> lightingChunks;

    for (const auto& chunkPosition : worldChunks)
    {
        int chunkX = std::get<0>(chunkPosition);
        int chunkY = std::get<1>(chunkPosition);
        int chunkZ = std::get<2>(chunkPosition);

        for (int offsetX = -1; offsetX <= 1; offsetX++)
        {
            for (int offsetY = -1; offsetY <= 1; offsetY++)
            {
                for (int offsetZ = -1; offsetZ <= 1; offsetZ++)
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

    auto isInsideLightingArea = [&](int x, int y, int z)
        {
            return lightingChunks.find(
                std::make_tuple(
                    getChunkCoordinate(x),
                    getChunkCoordinate(y),
                    getChunkCoordinate(z)
                )
            ) != lightingChunks.end();
        };

    // --------------------------------------------------------
    // Seed direct sunlight
    // --------------------------------------------------------

    for (const auto& chunkPosition : lightingChunks)
    {
        int startX = std::get<0>(chunkPosition) * CHUNK_SIZE;
        int startY = std::get<1>(chunkPosition) * CHUNK_SIZE;
        int startZ = std::get<2>(chunkPosition) * CHUNK_SIZE;

        for (int localX = 0; localX < CHUNK_SIZE; localX++)
        {
            int x = startX + localX;

            for (int localZ = 0; localZ < CHUNK_SIZE; localZ++)
            {
                int z = startZ + localZ;

                auto highestIterator = highestSolidBlock.find(
                    std::make_pair(x, z)
                );

                bool columnHasSolidBlock =
                    highestIterator != highestSolidBlock.end();

                int highestY = 0;

                if (columnHasSolidBlock)
                {
                    highestY = highestIterator->second;
                }

                for (int localY = 0; localY < CHUNK_SIZE; localY++)
                {
                    int y = startY + localY;

                    if (world.isSolidAt(x, y, z))
                    {
                        continue;
                    }

                    if (!columnHasSolidBlock)
                    {
                        setSkyLight(x, y, z, MAX_LIGHT);
                        continue;
                    }

                    if (y > highestY)
                    {
                        setSkyLight(x, y, z, MAX_LIGHT);
                    }
                }
            }
        }
    }

    // --------------------------------------------------------
    // Find bright cells beside darker air
    // --------------------------------------------------------

    std::queue<LightPosition> propagationQueue;

    const int neighborOffsets[6][3] =
    {
        { 1, 0, 0 },
        {-1, 0, 0 },
        { 0, 1, 0 },
        { 0,-1, 0 },
        { 0, 0, 1 },
        { 0, 0,-1 }
    };

    for (const auto& chunkPosition : lightingChunks)
    {
        int startX = std::get<0>(chunkPosition) * CHUNK_SIZE;
        int startY = std::get<1>(chunkPosition) * CHUNK_SIZE;
        int startZ = std::get<2>(chunkPosition) * CHUNK_SIZE;

        for (int localX = 0; localX < CHUNK_SIZE; localX++)
        {
            for (int localY = 0; localY < CHUNK_SIZE; localY++)
            {
                for (int localZ = 0; localZ < CHUNK_SIZE; localZ++)
                {
                    int x = startX + localX;
                    int y = startY + localY;
                    int z = startZ + localZ;

                    if (getSkyLight(x, y, z) != MAX_LIGHT)
                    {
                        continue;
                    }

                    bool touchesDarkAir = false;

                    for (int neighbor = 0; neighbor < 6; neighbor++)
                    {
                        int nx = x + neighborOffsets[neighbor][0];
                        int ny = y + neighborOffsets[neighbor][1];
                        int nz = z + neighborOffsets[neighbor][2];

                        if (!isInsideLightingArea(nx, ny, nz))
                        {
                            continue;
                        }

                        if (world.isSolidAt(nx, ny, nz))
                        {
                            continue;
                        }

                        if (getSkyLight(nx, ny, nz) < MAX_LIGHT - 1)
                        {
                            touchesDarkAir = true;
                            break;
                        }
                    }

                    if (touchesDarkAir)
                    {
                        propagationQueue.push({ x, y, z });
                    }
                }
            }
        }
    }

    // --------------------------------------------------------
    // Spread sunlight into shaded air
    // --------------------------------------------------------

    while (!propagationQueue.empty())
    {
        LightPosition current = propagationQueue.front();
        propagationQueue.pop();

        int currentLight = getSkyLight(
            current.x, current.y, current.z
        );

        if (currentLight <= 1)
        {
            continue;
        }

        int nextLight = currentLight - 1;

        for (int neighbor = 0; neighbor < 6; neighbor++)
        {
            int nx = current.x + neighborOffsets[neighbor][0];
            int ny = current.y + neighborOffsets[neighbor][1];
            int nz = current.z + neighborOffsets[neighbor][2];

            if (!isInsideLightingArea(nx, ny, nz))
            {
                continue;
            }

            if (world.isSolidAt(nx, ny, nz))
            {
                continue;
            }

            if (getSkyLight(nx, ny, nz) >= nextLight)
            {
                continue;
            }

            setSkyLight(nx, ny, nz, nextLight);
            propagationQueue.push({ nx, ny, nz });
        }
    }
}

void Lighting::calculateBlockLight(
    const World& world
)
{
    // Remove light from any bulbs that were broken.
    for (auto& chunkEntry : lightChunks)
    {
        chunkEntry.second.blockLight.fill(0);
    }

    std::queue<LightPosition> propagationQueue;

    const int neighborOffsets[6][3] =
    {
        { 1, 0, 0 },
        {-1, 0, 0 },
        { 0, 1, 0 },
        { 0,-1, 0 },
        { 0, 0, 1 },
        { 0, 0,-1 }
    };

    // Seed every light-emitting block.
    for (const auto& entry : world.blocks)
    {
        const Block& block = entry.second;

        if (block.emittedLight <= 0)
        {
            continue;
        }

        int x = std::get<0>(entry.first);
        int y = std::get<1>(entry.first);
        int z = std::get<2>(entry.first);

        setBlockLight(x, y, z, block.emittedLight);

        propagationQueue.push({ x, y, z });
    }

    // Spread light through air.
    while (!propagationQueue.empty())
    {
        LightPosition current = propagationQueue.front();
        propagationQueue.pop();

        int currentLight = getBlockLight(
            current.x,
            current.y,
            current.z
        );

        if (currentLight <= 1)
        {
            continue;
        }

        int nextLight = currentLight - 1;

        for (const auto& offset : neighborOffsets)
        {
            int nx = current.x + offset[0];
            int ny = current.y + offset[1];
            int nz = current.z + offset[2];

            if (world.isSolidAt(nx, ny, nz))
            {
                continue;
            }

            if (getBlockLight(nx, ny, nz) >= nextLight)
            {
                continue;
            }

            setBlockLight(nx, ny, nz, nextLight);

            propagationQueue.push({ nx, ny, nz });
        }
    }
}


// ============================================================
// Record meshes affected by a changed light cell
// ============================================================

void Lighting::addDirtyChunkForCell(
    std::set<std::tuple<int, int, int>>& dirtyChunks,
    int x,
    int y,
    int z
) const
{
    int chunkX = getChunkCoordinate(x);
    int chunkY = getChunkCoordinate(y);
    int chunkZ = getChunkCoordinate(z);

    int localX = getLocalCoordinate(x, chunkX);
    int localY = getLocalCoordinate(y, chunkY);
    int localZ = getLocalCoordinate(z, chunkZ);

    dirtyChunks.insert(
        std::make_tuple(chunkX, chunkY, chunkZ)
    );

    // A neighboring chunk may sample this cell at a shared edge.
    if (localX == 0)
    {
        dirtyChunks.insert(
            std::make_tuple(chunkX - 1, chunkY, chunkZ)
        );
    }

    if (localX == CHUNK_SIZE - 1)
    {
        dirtyChunks.insert(
            std::make_tuple(chunkX + 1, chunkY, chunkZ)
        );
    }

    if (localY == 0)
    {
        dirtyChunks.insert(
            std::make_tuple(chunkX, chunkY - 1, chunkZ)
        );
    }

    if (localY == CHUNK_SIZE - 1)
    {
        dirtyChunks.insert(
            std::make_tuple(chunkX, chunkY + 1, chunkZ)
        );
    }

    if (localZ == 0)
    {
        dirtyChunks.insert(
            std::make_tuple(chunkX, chunkY, chunkZ - 1)
        );
    }

    if (localZ == CHUNK_SIZE - 1)
    {
        dirtyChunks.insert(
            std::make_tuple(chunkX, chunkY, chunkZ + 1)
        );
    }
}


// ============================================================
// Check direct access to the sky
// ============================================================

int Lighting::findDirectSkyLight(
    const World& world,
    int x,
    int y,
    int z
) const
{
    if (world.isSolidAt(x, y, z))
    {
        return 0;
    }

    auto columnIterator = solidColumnHeights.find(
        std::make_pair(x, z)
    );

    if (columnIterator == solidColumnHeights.end())
    {
        return MAX_LIGHT;
    }

    if (columnIterator->second.empty())
    {
        return MAX_LIGHT;
    }

    int highestSolidY = *columnIterator->second.rbegin();

    if (y > highestSolidY)
    {
        return MAX_LIGHT;
    }

    return 0;
}


// ============================================================
// Spread newly available skylight
// ============================================================

void Lighting::propagateSkyLightAddition(
    const World& world,
    int startX,
    int startY,
    int startZ,
    std::set<std::tuple<int, int, int>>& dirtyChunks
)
{
    if (world.isSolidAt(startX, startY, startZ))
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
        { 1, 0, 0 },
        {-1, 0, 0 },
        { 0, 1, 0 },
        { 0,-1, 0 },
        { 0, 0, 1 },
        { 0, 0,-1 }
    };

    int bestLight = findDirectSkyLight(
        world, startX, startY, startZ
    );

    // Consider light entering from each neighboring air cell.
    for (int neighbor = 0; neighbor < 6; neighbor++)
    {
        int nx = startX + neighborOffsets[neighbor][0];
        int ny = startY + neighborOffsets[neighbor][1];
        int nz = startZ + neighborOffsets[neighbor][2];

        if (world.isSolidAt(nx, ny, nz))
        {
            continue;
        }

        int possibleLight = getSkyLight(nx, ny, nz) - 1;

        if (possibleLight > bestLight)
        {
            bestLight = possibleLight;
        }
    }

    bestLight = clampLight(bestLight);

    int existingStartLight = getSkyLight(
        startX, startY, startZ
    );

    if (bestLight > existingStartLight)
    {
        setSkyLight(startX, startY, startZ, bestLight);

        addDirtyChunkForCell(
            dirtyChunks, startX, startY, startZ
        );
    }

    if (bestLight <= 1)
    {
        return;
    }

    std::queue<LightNode> propagationQueue;
    propagationQueue.push({ startX, startY, startZ });

    while (!propagationQueue.empty())
    {
        LightNode current = propagationQueue.front();
        propagationQueue.pop();

        int currentLight = getSkyLight(
            current.x, current.y, current.z
        );

        if (currentLight <= 1)
        {
            continue;
        }

        int nextLight = currentLight - 1;

        for (int neighbor = 0; neighbor < 6; neighbor++)
        {
            int nx = current.x + neighborOffsets[neighbor][0];
            int ny = current.y + neighborOffsets[neighbor][1];
            int nz = current.z + neighborOffsets[neighbor][2];

            if (world.isSolidAt(nx, ny, nz))
            {
                continue;
            }

            // New regions must initialize their direct sunlight first.
            if (!findLightChunk(
                getChunkCoordinate(nx),
                getChunkCoordinate(ny),
                getChunkCoordinate(nz)
            ))
            {
                continue;
            }

            if (getSkyLight(nx, ny, nz) >= nextLight)
            {
                continue;
            }

            setSkyLight(nx, ny, nz, nextLight);
            addDirtyChunkForCell(dirtyChunks, nx, ny, nz);

            propagationQueue.push({ nx, ny, nz });
        }
    }
}


// ============================================================
// Remove invalid skylight and restore surviving light paths
// ============================================================

void Lighting::propagateSkyLightRemoval(
    const World& world,
    int startX,
    int startY,
    int startZ,
    int oldLight,
    std::set<std::tuple<int, int, int>>& dirtyChunks
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
        { 1, 0, 0 },
        {-1, 0, 0 },
        { 0, 1, 0 },
        { 0,-1, 0 },
        { 0, 0, 1 },
        { 0, 0,-1 }
    };

    std::queue<RemovalNode> removalQueue;
    std::set<std::tuple<int, int, int>> relightCells;

    removalQueue.push({ startX, startY, startZ, oldLight });

    while (!removalQueue.empty())
    {
        RemovalNode current = removalQueue.front();
        removalQueue.pop();

        for (int neighbor = 0; neighbor < 6; neighbor++)
        {
            int nx = current.x + neighborOffsets[neighbor][0];
            int ny = current.y + neighborOffsets[neighbor][1];
            int nz = current.z + neighborOffsets[neighbor][2];

            if (world.isSolidAt(nx, ny, nz))
            {
                continue;
            }

            int neighborLight = getSkyLight(nx, ny, nz);

            if (neighborLight <= 0)
            {
                continue;
            }

            // Direct sunlight is an independent source.
            if (findDirectSkyLight(world, nx, ny, nz) == MAX_LIGHT)
            {
                relightCells.insert(std::make_tuple(nx, ny, nz));
                continue;
            }

            if (neighborLight < current.oldLight)
            {
                setSkyLight(nx, ny, nz, 0);
                addDirtyChunkForCell(dirtyChunks, nx, ny, nz);

                removalQueue.push({ nx, ny, nz, neighborLight });
            }
            else
            {
                // Equal or brighter light may come from another path.
                relightCells.insert(std::make_tuple(nx, ny, nz));
            }
        }
    }

    for (const auto& position : relightCells)
    {
        propagateSkyLightAddition(
            world,
            std::get<0>(position),
            std::get<1>(position),
            std::get<2>(position),
            dirtyChunks
        );
    }
}


// ============================================================
// Update lighting after placing or removing a block
//
// The World must already contain the NEW block state.
// ============================================================

std::set<std::tuple<int, int, int>> Lighting::updateBlockChange(
    const World& world,
    int x,
    int y,
    int z
)
{
    std::set<std::tuple<int, int, int>> dirtyChunks;
    std::pair<int, int> column = std::make_pair(x, z);

    // The column cache still describes the previous world state.
    auto oldColumnIterator = solidColumnHeights.find(column);

    bool wasSolid = false;
    bool hadOldHighest = false;
    int oldHighest = 0;

    if (oldColumnIterator != solidColumnHeights.end())
    {
        wasSolid =
            oldColumnIterator->second.find(y) !=
            oldColumnIterator->second.end();

        if (!oldColumnIterator->second.empty())
        {
            hadOldHighest = true;
            oldHighest = *oldColumnIterator->second.rbegin();
        }
    }

    int changedCellOldLight = getSkyLight(x, y, z);
    bool isSolidNow = world.isSolidAt(x, y, z);

    // Update the cache to describe the new world state.
    if (isSolidNow)
    {
        solidColumnHeights[column].insert(y);
    }
    else
    {
        auto columnIterator = solidColumnHeights.find(column);

        if (columnIterator != solidColumnHeights.end())
        {
            columnIterator->second.erase(y);

            if (columnIterator->second.empty())
            {
                solidColumnHeights.erase(columnIterator);
            }
        }
    }

    // Collect already stored cells in the edited column.
    std::vector<int> columnCells;

    int columnChunkX = getChunkCoordinate(x);
    int columnChunkZ = getChunkCoordinate(z);

    for (const auto& chunkEntry : lightChunks)
    {
        int lightChunkX = std::get<0>(chunkEntry.first);
        int lightChunkY = std::get<1>(chunkEntry.first);
        int lightChunkZ = std::get<2>(chunkEntry.first);

        if (
            lightChunkX != columnChunkX ||
            lightChunkZ != columnChunkZ
            )
        {
            continue;
        }

        for (int localY = 0; localY < CHUNK_SIZE; localY++)
        {
            columnCells.push_back(
                lightChunkY * CHUNK_SIZE + localY
            );
        }
    }

    std::map<std::tuple<int, int, int>, int> removalSeeds;
    std::set<std::tuple<int, int, int>> additionSeeds;

    for (int cellY : columnCells)
    {
        bool oldCellSolid;

        if (cellY == y)
        {
            oldCellSolid = wasSolid;
        }
        else
        {
            oldCellSolid = world.isSolidAt(x, cellY, z);
        }

        bool oldDirectSky =
            !oldCellSolid &&
            (!hadOldHighest || cellY > oldHighest);

        bool newDirectSky =
            findDirectSkyLight(world, x, cellY, z) == MAX_LIGHT;

        // Sunlight was blocked.
        if (oldDirectSky && !newDirectSky)
        {
            int oldLight = getSkyLight(x, cellY, z);

            if (oldLight > 0)
            {
                setSkyLight(x, cellY, z, 0);
                addDirtyChunkForCell(dirtyChunks, x, cellY, z);

                removalSeeds[std::make_tuple(x, cellY, z)] =
                    oldLight;
            }
        }

        // Direct sunlight became available.
        if (!oldDirectSky && newDirectSky)
        {
            if (getSkyLight(x, cellY, z) < MAX_LIGHT)
            {
                setSkyLight(x, cellY, z, MAX_LIGHT);
                addDirtyChunkForCell(dirtyChunks, x, cellY, z);
            }

            additionSeeds.insert(std::make_tuple(x, cellY, z));
        }
    }

    // A newly placed solid block cannot contain skylight.
    if (!wasSolid && isSolidNow)
    {
        if (changedCellOldLight > 0)
        {
            setSkyLight(x, y, z, 0);
            addDirtyChunkForCell(dirtyChunks, x, y, z);

            removalSeeds[std::make_tuple(x, y, z)] =
                changedCellOldLight;
        }
    }

    for (const auto& removal : removalSeeds)
    {
        propagateSkyLightRemoval(
            world,
            std::get<0>(removal.first),
            std::get<1>(removal.first),
            std::get<2>(removal.first),
            removal.second,
            dirtyChunks
        );
    }

    // A removed block leaves air that may now receive light.
    if (wasSolid && !isSolidNow)
    {
        additionSeeds.insert(std::make_tuple(x, y, z));
    }

    for (const auto& addition : additionSeeds)
    {
        propagateSkyLightAddition(
            world,
            std::get<0>(addition),
            std::get<1>(addition),
            std::get<2>(addition),
            dirtyChunks
        );
    }

    // Initialize any new lighting regions around the edit.
    extendSkyLightArea(world, x, y, z, dirtyChunks);

    calculateBlockLight(world);

    return dirtyChunks;
}


// ============================================================
// Initialize lighting when building into new areas
// ============================================================

void Lighting::extendSkyLightArea(
    const World& world,
    int x,
    int y,
    int z,
    std::set<std::tuple<int, int, int>>& dirtyChunks
)
{
    std::vector<std::tuple<int, int, int>> newChunks;

    const int cx = getChunkCoordinate(x);
    const int cy = getChunkCoordinate(y);
    const int cz = getChunkCoordinate(z);

    // Allocate the edited chunk and one surrounding chunk.
    for (int dx = -1; dx <= 1; ++dx)
    {
        for (int dy = -1; dy <= 1; ++dy)
        {
            for (int dz = -1; dz <= 1; ++dz)
            {
                if (findLightChunk(cx + dx, cy + dy, cz + dz))
                {
                    continue;
                }

                getOrCreateLightChunk(cx + dx, cy + dy, cz + dz);
                newChunks.emplace_back(cx + dx, cy + dy, cz + dz);
            }
        }
    }

    if (newChunks.empty())
    {
        return;
    }

    const int offsets[6][3] =
    {
        { 1, 0, 0 },
        {-1, 0, 0 },
        { 0, 1, 0 },
        { 0,-1, 0 },
        { 0, 0, 1 },
        { 0, 0,-1 }
    };

    std::queue<LightPosition> queue;

    // --------------------------------------------------------
    // Seed ALL new cells with direct sky access first
    // --------------------------------------------------------

    for (const auto& chunk : newChunks)
    {
        const int sx = std::get<0>(chunk) * CHUNK_SIZE;
        const int sy = std::get<1>(chunk) * CHUNK_SIZE;
        const int sz = std::get<2>(chunk) * CHUNK_SIZE;

        for (int ax = sx; ax < sx + CHUNK_SIZE; ++ax)
        {
            for (int ay = sy; ay < sy + CHUNK_SIZE; ++ay)
            {
                for (int az = sz; az < sz + CHUNK_SIZE; ++az)
                {
                    if (
                        findDirectSkyLight(world, ax, ay, az) ==
                        MAX_LIGHT
                        )
                    {
                        setSkyLight(ax, ay, az, MAX_LIGHT);
                        addDirtyChunkForCell(
                            dirtyChunks, ax, ay, az
                        );
                    }
                }
            }
        }
    }

    // --------------------------------------------------------
    // Find sources beside darker air
    //
    // Include old neighbors so light can cross both ways.
    // --------------------------------------------------------

    for (const auto& chunk : newChunks)
    {
        const int sx = std::get<0>(chunk) * CHUNK_SIZE;
        const int sy = std::get<1>(chunk) * CHUNK_SIZE;
        const int sz = std::get<2>(chunk) * CHUNK_SIZE;

        for (int ax = sx; ax < sx + CHUNK_SIZE; ++ax)
        {
            for (int ay = sy; ay < sy + CHUNK_SIZE; ++ay)
            {
                for (int az = sz; az < sz + CHUNK_SIZE; ++az)
                {
                    if (world.isSolidAt(ax, ay, az))
                    {
                        continue;
                    }

                    const int light = getSkyLight(ax, ay, az);

                    for (const auto& offset : offsets)
                    {
                        const int nx = ax + offset[0];
                        const int ny = ay + offset[1];
                        const int nz = az + offset[2];

                        if (
                            !findLightChunk(
                                getChunkCoordinate(nx),
                                getChunkCoordinate(ny),
                                getChunkCoordinate(nz)
                            ) ||
                            world.isSolidAt(nx, ny, nz)
                            )
                        {
                            continue;
                        }

                        const int neighbor = getSkyLight(nx, ny, nz);

                        if (light > neighbor + 1)
                        {
                            queue.push({ ax, ay, az });
                        }

                        if (neighbor > light + 1)
                        {
                            queue.push({ nx, ny, nz });
                        }
                    }
                }
            }
        }
    }

    // --------------------------------------------------------
    // Spread light through the initialized region
    // --------------------------------------------------------

    while (!queue.empty())
    {
        const LightPosition cell = queue.front();
        queue.pop();

        const int nextLight =
            getSkyLight(cell.x, cell.y, cell.z) - 1;

        if (nextLight <= 0)
        {
            continue;
        }

        for (const auto& offset : offsets)
        {
            const int nx = cell.x + offset[0];
            const int ny = cell.y + offset[1];
            const int nz = cell.z + offset[2];

            if (
                !findLightChunk(
                    getChunkCoordinate(nx),
                    getChunkCoordinate(ny),
                    getChunkCoordinate(nz)
                ) ||
                world.isSolidAt(nx, ny, nz) ||
                getSkyLight(nx, ny, nz) >= nextLight
                )
            {
                continue;
            }

            setSkyLight(nx, ny, nz, nextLight);
            addDirtyChunkForCell(dirtyChunks, nx, ny, nz);

            queue.push({ nx, ny, nz });
        }
    }
}