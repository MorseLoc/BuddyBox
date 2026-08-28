#include "NPC.h"

#include "World.h"

#include <cmath>
#include <cstdlib>
#include <vector>


// ============================================================
// NPC constructor
//
// Creates one NPC at the requested starting position.
// ============================================================

NPC::NPC(
    NPCType npcType,
    const glm::vec3& startPosition
)
{
    type = npcType;

    position = startPosition;

    targetPosition = startPosition;

    hasTarget = false;


    // Jebub movement speed.
    speed = 1.5f;


    // Starting facing direction.
    facingYaw = 0.0f;


    // Jebub collision box.
    //
    // Slightly smaller than a full block.
    size = glm::vec3(
        0.8f,
        1.3f,
        0.8f
    );


    // Start with no vertical movement.
    verticalVelocity = 0.0f;


    // Collision will determine whether
    // the NPC is standing on the ground.
    grounded = false;


    // Start stuck detection from
    // the NPC's starting position.
    lastPosition =
        position;


    // NPC starts with no stuck time.
    stuckTimer =
        0.0f;


    // Walking animation starts at the beginning.
    walkAnimationTime =
        0.0f;


    // NPC starts without walking.
    moving =
        false;
}


// ============================================================
// Update NPC
//
// Called once every frame.
//
// Handles:
// - Choosing a target
// - Walking
// - Walking animation
// - Stuck detection
// - Gravity
// - Vertical collision
// ============================================================

void NPC::update(
    float deltaTime,
    const World& world
)
{
    // --------------------------------------------------------
    // Choose a destination
    // --------------------------------------------------------

    if (!hasTarget)
    {
        chooseRandomTarget(
            world
        );
    }


    // Assume the NPC is not moving.
    //
    // moveTowardTarget() will change this to true
    // if the NPC is currently trying to walk.
    moving =
        false;


    // --------------------------------------------------------
    // Walk toward destination
    // --------------------------------------------------------

    if (hasTarget)
    {
        moveTowardTarget(
            deltaTime,
            world
        );
    }


    // --------------------------------------------------------
    // Walking animation
    // --------------------------------------------------------

    if (moving)
    {
        // Advance the walking animation
        // while the NPC is moving.
        walkAnimationTime +=
            deltaTime;
    }
    else
    {
        // Reset the animation when standing still
        // so the legs return to resting position.
        walkAnimationTime =
            0.0f;
    }


    // --------------------------------------------------------
    // Stuck detection
    // --------------------------------------------------------

    glm::vec3 movementSinceLastFrame =
        position - lastPosition;


    // Ignore vertical movement.
    movementSinceLastFrame.y =
        0.0f;


    float movedDistance =
        glm::length(
            movementSinceLastFrame
        );


    // If the NPC has a target but is barely moving,
    // count how long it has been stuck.
    if (
        hasTarget &&
        movedDistance < 0.005f
        )
    {
        stuckTimer +=
            deltaTime;
    }
    else
    {
        stuckTimer =
            0.0f;
    }


    // Save current position for the next frame.
    lastPosition =
        position;


    // Give up on this target after one second
    // without meaningful movement.
    if (stuckTimer >= 1.0f)
    {
        hasTarget =
            false;


        stuckTimer =
            0.0f;
    }


    // --------------------------------------------------------
    // Gravity
    // --------------------------------------------------------

    const float gravity =
        -23.0f;


    verticalVelocity +=
        gravity * deltaTime;


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
        position.y =
            verticalTestPosition.y;


        grounded =
            false;
    }
    else
    {
        // A collision while falling means
        // the NPC landed on something.
        if (verticalVelocity < 0.0f)
        {
            grounded =
                true;
        }
        else
        {
            grounded =
                false;
        }


        verticalVelocity =
            0.0f;
    }
}


// ============================================================
// Choose random target
//
// Looks for valid blocks within 3 blocks of the NPC
// and randomly chooses one.
//
// For now, Jebubs only choose destinations
// on roughly the same height level.
// ============================================================

void NPC::chooseRandomTarget(
    const World& world
)
{
    std::vector<glm::vec3> possibleTargets;


    int startX =
        static_cast<int>(
            std::floor(
                position.x + 0.5f
            )
            );


    int startY =
        static_cast<int>(
            std::floor(
                position.y + 0.5f
            )
            );


    int startZ =
        static_cast<int>(
            std::floor(
                position.z + 0.5f
            )
            );


    // Search up to 3 blocks away horizontally.
    for (int offsetX = -3; offsetX <= 3; offsetX++)
    {
        for (int offsetZ = -3; offsetZ <= 3; offsetZ++)
        {
            // Skip the NPC's current position.
            if (
                offsetX == 0 &&
                offsetZ == 0
                )
            {
                continue;
            }


            int targetX =
                startX + offsetX;


            int targetZ =
                startZ + offsetZ;


            // The block underneath the NPC's feet.
            int groundY =
                startY - 1;


            // A valid destination needs:
            //
            // 1. Solid ground underneath.
            // 2. Empty space where the NPC will stand.
            if (
                world.isSolidAt(
                    targetX,
                    groundY,
                    targetZ
                )
                &&
                !world.hasBlock(
                    targetX,
                    startY,
                    targetZ
                )
                )
            {
                possibleTargets.push_back(
                    glm::vec3(
                        static_cast<float>(
                            targetX
                            ),

                        position.y,

                        static_cast<float>(
                            targetZ
                            )
                    )
                );
            }
        }
    }


    // No valid destination found.
    if (possibleTargets.empty())
    {
        hasTarget =
            false;

        return;
    }


    // Pick one valid target randomly.
    int randomIndex =
        std::rand() %
        static_cast<int>(
            possibleTargets.size()
            );


    targetPosition =
        possibleTargets[
            randomIndex
        ];


    hasTarget =
        true;
}


// ============================================================
// Move toward target
//
// Walks in a straight line toward targetPosition.
//
// X and Z collision are checked separately so the NPC
// can slide along walls instead of getting completely stuck.
// ============================================================

void NPC::moveTowardTarget(
    float deltaTime,
    const World& world
)
{
    glm::vec3 direction =
        targetPosition -
        position;


    // Jebubs only walk horizontally.
    direction.y =
        0.0f;


    float distance =
        glm::length(
            direction
        );


    // Close enough to count as arriving.
    if (distance < 0.05f)
    {
        hasTarget =
            false;

        return;
    }


    direction =
        glm::normalize(
            direction
        );


    // Turn the NPC toward the direction
    // it is currently walking.
    facingYaw =
        glm::degrees(
            std::atan2(
                direction.x,
                direction.z
            )
        );


    glm::vec3 movement =
        direction *
        speed *
        deltaTime;


    // The NPC is actively trying to walk.
    moving =
        true;


    // --------------------------------------------------------
    // X-axis movement
    // --------------------------------------------------------

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
    // Z-axis movement
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
    // Check whether the target was reached
    // --------------------------------------------------------

    glm::vec3 remainingDistance =
        targetPosition -
        position;


    remainingDistance.y =
        0.0f;


    if (
        glm::length(
            remainingDistance
        )
        <
        0.05f
        )
    {
        hasTarget =
            false;
    }
}


// ============================================================
// World collision
//
// Checks whether the NPC's collision box would overlap
// any solid block at testPosition.
//
// Returns:
// true  = collision
// false = position is clear
// ============================================================

bool NPC::collidesWithWorld(
    const glm::vec3& testPosition,
    const World& world
) const
{
    glm::vec3 npcMin =
        testPosition -
        (size / 2.0f);


    glm::vec3 npcMax =
        testPosition +
        (size / 2.0f);


    int minX =
        static_cast<int>(
            std::floor(
                npcMin.x + 0.5f
            )
            );


    int maxX =
        static_cast<int>(
            std::floor(
                npcMax.x + 0.5f
            )
            );


    int minY =
        static_cast<int>(
            std::floor(
                npcMin.y + 0.5f
            )
            );


    int maxY =
        static_cast<int>(
            std::floor(
                npcMax.y + 0.5f
            )
            );


    int minZ =
        static_cast<int>(
            std::floor(
                npcMin.z + 0.5f
            )
            );


    int maxZ =
        static_cast<int>(
            std::floor(
                npcMax.z + 0.5f
            )
            );


    for (int x = minX; x <= maxX; x++)
    {
        for (int y = minY; y <= maxY; y++)
        {
            for (int z = minZ; z <= maxZ; z++)
            {
                if (world.isSolidAt(
                    x,
                    y,
                    z
                ))
                {
                    return true;
                }
            }
        }
    }


    return false;
}


// ============================================================
// Get NPC position
// ============================================================

const glm::vec3& NPC::getPosition() const
{
    return position;
}


// ============================================================
// Get NPC type
// ============================================================

NPCType NPC::getType() const
{
    return type;
}


// ============================================================
// Get NPC facing direction
// ============================================================

float NPC::getFacingYaw() const
{
    return facingYaw;
}


// ============================================================
// Get walking animation time
// ============================================================

float NPC::getWalkAnimationTime() const
{
    return walkAnimationTime;
}


// ============================================================
// Is NPC moving
// ============================================================

bool NPC::isMoving() const
{
    return moving;
}