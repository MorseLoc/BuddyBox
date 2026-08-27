#pragma once

#include <glm/glm.hpp>


// Forward declarations.
//
// These tell C++ that these types exist without forcing
// Player.h to include their full header files.
struct GLFWwindow;
struct World;


// ============================================================
// Player
//
// Represents the player inside the BuddyBox world.
//
// The Player stores:
// - Position
// - Collision-box size
// - Walking speed
// - Vertical movement
// - Grounded state
//
// Player.cpp handles movement, gravity, jumping,
// and collision with the world.
// ============================================================

struct Player
{
    // --------------------------------------------------------
    // Position and size
    // --------------------------------------------------------

    // Center position of the player in the 3D world.
    //
    // X = left / right
    // Y = up / down
    // Z = forward / backward
    glm::vec3 position;


    // Size of the player's invisible collision box.
    //
    // X = width
    // Y = height
    // Z = depth
    glm::vec3 size;


    // --------------------------------------------------------
    // Movement
    // --------------------------------------------------------

    // Horizontal walking speed.
    // Measured in world units per second.
    float speed;


    // Current movement speed on the Y axis.
    //
    // Positive = moving upward
    // Negative = falling
    // Zero     = no vertical movement
    float verticalVelocity;


    // true when the player is standing on solid ground.
    bool grounded;


    // --------------------------------------------------------
    // Constructor
    // --------------------------------------------------------

    // Creates a player with the default values
    // defined in Player.cpp.
    Player();


    // --------------------------------------------------------
    // Collision
    // --------------------------------------------------------

    // Tests whether the player's collision box would overlap
    // any solid block at testPosition.
    //
    // Returns:
    // true  = collision
    // false = position is clear
    bool collidesWithWorld(
        const glm::vec3& testPosition,
        const World& world
    ) const;


    // --------------------------------------------------------
    // Movement update
    // --------------------------------------------------------

    // Handles one frame of player movement.
    //
    // This includes:
    // - WASD movement
    // - Horizontal collision
    // - Jumping
    // - Gravity
    // - Vertical collision
    //
    // deltaTime keeps movement independent of frame rate.
    //
    // cameraFront tells the player which direction is forward.
    // cameraUp helps calculate the sideways direction.
    void move(
        GLFWwindow* window,
        float deltaTime,
        const glm::vec3& cameraFront,
        const glm::vec3& cameraUp,
        const World& world
    );
};