#pragma once

#include <map>
#include <tuple>


struct World;


// ============================================================
// Lighting
//
// Handles BuddyBox lighting.
//
// Lighting values use:
//
// 15 = fully lit
// 0  = completely dark
//
// Later this class will handle:
//
// - Sunlight
// - Light propagation
// - Light-emitting blocks
// - Day / night brightness
// ============================================================

class Lighting
{
public:

    // --------------------------------------------------------
    // Light constants
    // --------------------------------------------------------

    static const int MAX_LIGHT =
        15;


    // --------------------------------------------------------
    // Sunlight
    // --------------------------------------------------------

    // Calculates sunlight for the world.
//
// For now:
//
// 15 = directly exposed to sky
// 0  = covered
//
// Later we will propagate lower light
// values into covered areas.
    void calculateSkyLight(
        const World& world
    );

    // Returns true if this position has
    // an unobstructed path to the sky.
    bool isExposedToSky(
        const World& world,
        int x,
        int y,
        int z
    ) const;


    // --------------------------------------------------------
    // Sky light storage
    // --------------------------------------------------------

    // Sets the skylight value at a position.
    void setSkyLight(
        int x,
        int y,
        int z,
        int lightLevel
    );


    // Returns the skylight value at a position.
    //
    // Positions without stored light
    // currently return 0.
    int getSkyLight(
        int x,
        int y,
        int z
    ) const;


    // Removes all stored skylight values.
    void clearSkyLight();


private:

    // --------------------------------------------------------
    // Stored sky light
    // --------------------------------------------------------

    std::map<
        std::tuple<int, int, int>,
        int
    > skyLight;
};