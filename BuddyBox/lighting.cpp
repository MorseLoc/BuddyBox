#include "Lighting.h"

#include "World.h"

#include <tuple>
#include <map>


// ============================================================
// Is exposed to sky
//
// Returns true when no solid block exists
// above this position.
//
// This gives us the starting points for sunlight.
//
// Exposed positions will eventually receive:
// skyLight = 15
// ============================================================

bool Lighting::isExposedToSky(
    const World& world,
    int x,
    int y,
    int z
) const
{
    // Check every block currently stored in the world.
    for (const auto& entry : world.blocks)
    {
        int blockX =
            std::get<0>(
                entry.first
            );


        int blockY =
            std::get<1>(
                entry.first
            );


        int blockZ =
            std::get<2>(
                entry.first
            );


        // Only blocks in the same vertical
        // X/Z column can block the sun.
        if (
            blockX != x ||
            blockZ != z
            )
        {
            continue;
        }


        // Ignore blocks at or below
        // the tested position.
        if (
            blockY <= y
            )
        {
            continue;
        }


        // Solid blocks stop sunlight.
        if (
            entry.second.solid
            )
        {
            return false;
        }
    }


    // Nothing solid was found above.
    return true;
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
    // Keep light inside the valid range.
    if (
        lightLevel <
        0
        )
    {
        lightLevel =
            0;
    }


    if (
        lightLevel >
        MAX_LIGHT
        )
    {
        lightLevel =
            MAX_LIGHT;
    }


    skyLight[
        std::make_tuple(
            x,
            y,
            z
        )
    ] =
        lightLevel;
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
    auto lightIterator =
        skyLight.find(
            std::make_tuple(
                x,
                y,
                z
            )
        );


    // No light stored here yet.
    if (
        lightIterator ==
        skyLight.end()
        )
    {
        return 0;
    }


    return
        lightIterator->second;
}


// ============================================================
// Clear sky light
//
// Used whenever we need to completely
// recalculate the world's sunlight.
// ============================================================

void Lighting::clearSkyLight()
{
    skyLight.clear();
}

// ============================================================
// Calculate sky light
//
// Calculates direct sunlight for the world.
//
// Instead of checking every block against every other block,
// we first find the highest solid block in each X/Z column.
//
// This makes sunlight calculation dramatically faster for
// large worlds.
// ============================================================

void Lighting::calculateSkyLight(
    const World& world
)
{
    // Remove old lighting information.

    clearSkyLight();


    // --------------------------------------------------------
    // Highest solid block in each X/Z column
    //
    // Key:
    //     X, Z
    //
    // Value:
    //     highest solid Y
    // --------------------------------------------------------

    std::map<
        std::pair<int, int>,
        int
    > highestSolidBlock;


    // ========================================================
    // PASS 1
    //
    // Find the highest solid block in every X/Z column.
    // ========================================================

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


        const Block& block =
            entry.second;


        // Non-solid blocks do not block sunlight.

        if (!block.solid)
        {
            continue;
        }


        std::pair<int, int> column =
            std::make_pair(
                x,
                z
            );


        auto columnIterator =
            highestSolidBlock.find(
                column
            );


        // We have not seen this column yet.

        if (
            columnIterator ==
            highestSolidBlock.end()
            )
        {
            highestSolidBlock[
                column
            ] =
                y;

                continue;
        }


        // This block is higher than the
        // previous highest block.

        if (
            y >
            columnIterator->second
            )
        {
            columnIterator->second =
                y;
        }
    }


    // ========================================================
    // PASS 2
    //
    // Give each block its direct sky light value.
    // ========================================================

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


        std::pair<int, int> column =
            std::make_pair(
                x,
                z
            );


        auto columnIterator =
            highestSolidBlock.find(
                column
            );


        // ----------------------------------------------------
        // No solid block exists in this column.
        //
        // Nothing blocks the sky.
        // ----------------------------------------------------

        if (
            columnIterator ==
            highestSolidBlock.end()
            )
        {
            setSkyLight(
                x,
                y,
                z,
                MAX_LIGHT
            );

            continue;
        }


        int highestY =
            columnIterator->second;


        // ----------------------------------------------------
        // If this block is the highest solid block
        // in its column, nothing solid exists above it.
        //
        // Therefore it receives full direct sunlight.
        // ----------------------------------------------------

        if (
            y >=
            highestY
            )
        {
            setSkyLight(
                x,
                y,
                z,
                MAX_LIGHT
            );
        }
        else
        {
            // Something solid exists above this block.

            setSkyLight(
                x,
                y,
                z,
                0
            );
        }
    }
}