#include "player.h"

#include <GLFW/glfw3.h>
#include <cmath>
#include <algorithm>

#include "world.h"


// ============================================================
// Constructor
// ============================================================

Player::Player()
{
    // Starting position.
    position = glm::vec3(
        0.0f,
        1.4f,
        3.0f
    );

    // Collision box: width, height, depth.
    size = glm::vec3(
        0.6f,
        1.8f,
        0.6f
    );

    speed = 6.0f;
    breakSpeed = 1.0f;

    verticalVelocity = 0.0f;
    grounded = false;
}


// ============================================================
// Check collision at a proposed position
// ============================================================

bool Player::collidesWithWorld(
    const glm::vec3& testPosition,
    const World& world
) const
{
    // Keep the whole player collision box inside the world.
    float halfWidth = size.x * 0.5f;
    float halfDepth = size.z * 0.5f;

    if (
        testPosition.x - halfWidth < -WORLD_BORDER ||
        testPosition.x + halfWidth > WORLD_BORDER ||
        testPosition.z - halfDepth < -WORLD_BORDER ||
        testPosition.z + halfDepth > WORLD_BORDER
        )
    {
        return true;
    }

    // Find the edges of the player's collision box.
    glm::vec3 playerMin = testPosition - (size / 2.0f);
    glm::vec3 playerMax = testPosition + (size / 2.0f);

    // Blocks are centered on whole-number coordinates.
    int minX = static_cast<int>(
        std::floor(playerMin.x + 0.5f)
        );

    int maxX = static_cast<int>(
        std::floor(playerMax.x + 0.5f)
        );

    int minY = static_cast<int>(
        std::floor(playerMin.y + 0.5f)
        );

    int maxY = static_cast<int>(
        std::floor(playerMax.y + 0.5f)
        );

    int minZ = static_cast<int>(
        std::floor(playerMin.z + 0.5f)
        );

    int maxZ = static_cast<int>(
        std::floor(playerMax.z + 0.5f)
        );

    // Check every grid cell touched by the collision box.
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

    return false;
}


// ============================================================
// Movement, jumping, and gravity
// ============================================================

void Player::move(
    GLFWwindow* window,
    float deltaTime,
    const glm::vec3& cameraFront,
    const glm::vec3& cameraUp,
    const World& world,
    bool acceptInput
)
{
    // --------------------------------------------------------
    // Walking directions
    // --------------------------------------------------------

    // Remove vertical camera tilt from walking.
    glm::vec3 flatFront = glm::normalize(
        glm::vec3(
            cameraFront.x,
            0.0f,
            cameraFront.z
        )
    );

    glm::vec3 rightDirection = glm::normalize(
        glm::cross(flatFront, cameraUp)
    );

    glm::vec3 movement(0.0f, 0.0f, 0.0f);

    // --------------------------------------------------------
    // Keyboard input
    // --------------------------------------------------------

    if (acceptInput && glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        movement += flatFront;
    }

    if (acceptInput && glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        movement -= flatFront;
    }

    if (acceptInput && glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        movement -= rightDirection;
    }

    if (acceptInput && glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        movement += rightDirection;
    }

    if (glm::length(movement) > 0.0f)
    {
        // Prevent diagonal movement from being faster.
        movement = glm::normalize(movement);

        // Convert speed into distance for this frame.
        movement *= speed * deltaTime;
    }

    // --------------------------------------------------------
    // Move along one axis without skipping through blocks
    //
    // axis 0 = X
    // axis 1 = Y
    // axis 2 = Z
    //
    // Returns true when movement hits a solid block.
    // --------------------------------------------------------

    auto moveAxis = [&](int axis, float distance)
        {
            // Split movement into pieces no longer than 0.25 blocks.
            // Even a fast fall must check the cells along its path.
            const int steps = std::max(
                1,
                static_cast<int>(
                    std::ceil(std::abs(distance) / 0.25f)
                    )
            );

            const float stepDistance =
                distance / static_cast<float>(steps);

            for (int step = 0; step < steps; ++step)
            {
                glm::vec3 next = position;
                next[axis] += stepDistance;

                if (!collidesWithWorld(next, world))
                {
                    position = next;
                    continue;
                }

                // This step hit something.
                // Find how much of the step is still safe to move.
                float clearFraction = 0.0f;
                float blockedFraction = 1.0f;

                for (int search = 0; search < 12; ++search)
                {
                    const float fraction =
                        (clearFraction + blockedFraction) * 0.5f;

                    next = position;
                    next[axis] += stepDistance * fraction;

                    if (collidesWithWorld(next, world))
                    {
                        blockedFraction = fraction;
                    }
                    else
                    {
                        clearFraction = fraction;
                    }
                }

                // Stop close to the surface without entering it.
                position[axis] += stepDistance * clearFraction;

                return true;
            }

            return false;
        };

    // Separate horizontal axes allow sliding along walls.
    moveAxis(0, movement.x);
    moveAxis(2, movement.z);

    // --------------------------------------------------------
    // Jumping
    // --------------------------------------------------------

    if (
        acceptInput &&
        glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS &&
        grounded
        )
    {
        const float jumpSpeed = 8.0f;

        verticalVelocity = jumpSpeed;
        grounded = false;
    }

    // --------------------------------------------------------
    // Gravity and vertical collision
    // --------------------------------------------------------

    const float gravity = -23.0f;

    verticalVelocity += gravity * deltaTime;

    const bool hitVertical = moveAxis(
        1,
        verticalVelocity * deltaTime
    );

    // A downward collision means the player landed.
    grounded = hitVertical && verticalVelocity < 0.0f;

    // Stop falling or rising after hitting a surface.
    if (hitVertical)
    {
        verticalVelocity = 0.0f;
    }
}