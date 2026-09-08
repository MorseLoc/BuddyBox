#pragma once

#include <glm/glm.hpp>


// These types are defined elsewhere.
struct GLFWwindow;
struct World;


// ============================================================
// Player
//
// Stores the player's position, collision box, and movement.
// ============================================================

struct Player
{
    // --------------------------------------------------------
    // Position and size
    // --------------------------------------------------------

    // Center of the player in world coordinates.
    glm::vec3 position;

    // Collision-box width, height, and depth.
    glm::vec3 size;


    // --------------------------------------------------------
    // Movement properties
    // --------------------------------------------------------

    // Walking speed in world units per second.
    float speed;

    // Seconds needed per point of block durability.
    float breakSpeed;

    // Positive = rising.
    // Negative = falling.
    float verticalVelocity;

    // True when standing on solid ground.
    bool grounded;


    // --------------------------------------------------------
    // Constructor
    // --------------------------------------------------------

    Player();


    // --------------------------------------------------------
    // Collision
    // --------------------------------------------------------

    // Returns true if the player's collision box at
    // testPosition overlaps a solid block.
    bool collidesWithWorld(
        const glm::vec3& testPosition,
        const World& world
    ) const;


    // --------------------------------------------------------
    // Movement update
    // --------------------------------------------------------

    // Handles walking, jumping, gravity, and collision.
    //
    // deltaTime:
    //     Time elapsed for this update, in seconds.
    //
    // cameraFront / cameraUp:
    //     Used to calculate walking directions.
    //
    // acceptInput:
    //     False disables walking and jumping input.
    //     Gravity and collision continue working.
    void move(
        GLFWwindow* window,
        float deltaTime,
        const glm::vec3& cameraFront,
        const glm::vec3& cameraUp,
        const World& world,
        bool acceptInput = true
    );
};