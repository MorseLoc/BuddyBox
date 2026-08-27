#include "Player.h"

#include <GLFW/glfw3.h>
#include <cmath>

#include "World.h"


// ============================================================
// Player constructor
//
// Creates the player and sets their starting properties.
// ============================================================

Player::Player()
{
    // Starting position in the world.
    position = glm::vec3(
        0.0f,
        1.4f,
        3.0f
    );


    // Player collision box.
    //
    // Width:  0.6 blocks
    // Height: 1.8 blocks
    // Depth:  0.6 blocks
    size = glm::vec3(
        0.6f,
        1.8f,
        0.6f
    );


    // Walking speed in world units per second.
    speed = 6.0f;


    // The player begins with no upward or downward movement.
    verticalVelocity = 0.0f;


    // Collision will determine whether the player
    // is standing on something.
    grounded = false;
}


// ============================================================
// World collision
//
// Checks whether the player's collision box would overlap
// any solid block at a given position.
//
// Returns:
// true  = collision
// false = position is clear
// ============================================================

bool Player::collidesWithWorld(
    const glm::vec3& testPosition,
    const World& world
) const
{
    // Calculate the minimum and maximum edges
    // of the player's collision box.
    glm::vec3 playerMin =
        testPosition - (size / 2.0f);

    glm::vec3 playerMax =
        testPosition + (size / 2.0f);


    // Convert the player's hitbox edges into
    // BuddyBox block-grid coordinates.
    //
    // Blocks are centered on whole-number positions,
    // so adding 0.5 before floor() finds the grid cell
    // containing each edge.
    int minX =
        static_cast<int>(
            std::floor(playerMin.x + 0.5f)
            );

    int maxX =
        static_cast<int>(
            std::floor(playerMax.x + 0.5f)
            );


    int minY =
        static_cast<int>(
            std::floor(playerMin.y + 0.5f)
            );

    int maxY =
        static_cast<int>(
            std::floor(playerMax.y + 0.5f)
            );


    int minZ =
        static_cast<int>(
            std::floor(playerMin.z + 0.5f)
            );

    int maxZ =
        static_cast<int>(
            std::floor(playerMax.z + 0.5f)
            );


    // Check every block-grid cell touched
    // by the player's collision box.
    for (int x = minX; x <= maxX; x++)
    {
        for (int y = minY; y <= maxY; y++)
        {
            for (int z = minZ; z <= maxZ; z++)
            {
                if (world.isSolidAt(x, y, z))
                {
                    return true;
                }
            }
        }
    }


    // No solid blocks overlap the player.
    return false;
}


// ============================================================
// Player movement
//
// Handles:
// - WASD movement
// - Horizontal collision
// - Jumping
// - Gravity
// - Vertical collision
// ============================================================

void Player::move(
    GLFWwindow* window,
    float deltaTime,
    const glm::vec3& cameraFront,
    const glm::vec3& cameraUp,
    const World& world
)
{
    // --------------------------------------------------------
    // Horizontal movement directions
    // --------------------------------------------------------

    // Remove the vertical part of the camera direction.
    //
    // This lets the player look up and down without
    // causing W/S movement to fly into the air or ground.
    glm::vec3 flatFront =
        glm::normalize(
            glm::vec3(
                cameraFront.x,
                0.0f,
                cameraFront.z
            )
        );


    // Calculate the direction directly to the player's right.
    glm::vec3 rightDirection =
        glm::normalize(
            glm::cross(
                flatFront,
                cameraUp
            )
        );


    // Start each frame with no requested movement.
    glm::vec3 movement(
        0.0f,
        0.0f,
        0.0f
    );


    // --------------------------------------------------------
    // WASD input
    // --------------------------------------------------------

    // W = forward
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        movement += flatFront;
    }


    // S = backward
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        movement -= flatFront;
    }


    // A = left
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        movement -= rightDirection;
    }


    // D = right
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        movement += rightDirection;
    }


    // --------------------------------------------------------
    // Movement speed
    // --------------------------------------------------------

    if (glm::length(movement) > 0.0f)
    {
        // Convert movement into a direction with length 1.
        //
        // Without this, moving diagonally would be
        // faster than moving straight.
        movement =
            glm::normalize(movement);


        // Convert direction into actual movement distance
        // for this frame.
        movement *=
            speed * deltaTime;
    }


    // --------------------------------------------------------
    // X-axis collision
    // --------------------------------------------------------

    // Test X movement separately.
    //
    // Separating X and Z allows the player to slide
    // along walls instead of becoming stuck against them.
    glm::vec3 testPosition =
        position;


    testPosition.x +=
        movement.x;


    if (!collidesWithWorld(
        testPosition,
        world
    ))
    {
        position.x =
            testPosition.x;
    }


    // --------------------------------------------------------
    // Z-axis collision
    // --------------------------------------------------------

    testPosition =
        position;


    testPosition.z +=
        movement.z;


    if (!collidesWithWorld(
        testPosition,
        world
    ))
    {
        position.z =
            testPosition.z;
    }


    // --------------------------------------------------------
    // Jumping
    // --------------------------------------------------------

    // The player can only begin a jump while grounded.
    if (
        glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS &&
        grounded
        )
    {
        const float jumpSpeed =
            8.0f;


        // Positive Y velocity launches the player upward.
        verticalVelocity =
            jumpSpeed;


        grounded =
            false;
    }


    // --------------------------------------------------------
    // Gravity
    // --------------------------------------------------------

    // Negative gravity constantly pulls the player's
    // vertical velocity downward.
    const float gravity =
        -23.0f;


    verticalVelocity +=
        gravity * deltaTime;


    // Calculate where vertical velocity wants
    // the player to move this frame.
    glm::vec3 verticalTestPosition =
        position;


    verticalTestPosition.y +=
        verticalVelocity * deltaTime;


    // --------------------------------------------------------
    // Vertical collision
    // --------------------------------------------------------

    if (!collidesWithWorld(
        verticalTestPosition,
        world
    ))
    {
        // Nothing blocks the movement.
        position.y =
            verticalTestPosition.y;


        grounded =
            false;
    }
    else
    {
        // A collision while falling means we landed.
        if (verticalVelocity < 0.0f)
        {
            grounded =
                true;
        }
        else
        {
            // A collision while moving upward means
            // the player hit their head on something.
            grounded =
                false;
        }


        // Stop vertical movement after hitting something.
        verticalVelocity =
            0.0f;
    }
}