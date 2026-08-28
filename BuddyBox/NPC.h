#pragma once

#include <glm/glm.hpp>


// Forward declaration.
//
// NPC only needs references to World,
// so we do not need to include World.h here.
struct World;


// ============================================================
// NPC types
//
// Lists the different kinds of NPCs
// that can exist in BuddyBox.
//
// Jebub is the first NPC type.
// More types can be added here later.
// ============================================================

enum class NPCType
{
    Jebub
};


// ============================================================
// NPC
//
// Represents one non-player creature in the world.
//
// NPC.cpp handles:
// - Target selection
// - Walking
// - Gravity
// - Block collision
//
// Different NPCType values can eventually
// have different behaviors and appearances.
// ============================================================

class NPC
{
public:
    // --------------------------------------------------------
    // Constructor
    // --------------------------------------------------------

    // Creates an NPC of the requested type
    // at the requested world position.
    NPC(
        NPCType type,
        const glm::vec3& startPosition
    );


    // --------------------------------------------------------
    // Update
    // --------------------------------------------------------

    // Updates this NPC once per frame.
    //
    // Handles:
    // - Choosing a destination
    // - Walking
    // - Gravity
    // - Collision
    void update(
        float deltaTime,
        const World& world
    );


    // --------------------------------------------------------
    // NPC information
    // --------------------------------------------------------

    // Returns the NPC's current world position.
    const glm::vec3& getPosition() const;


    // Returns what kind of NPC this is.
    NPCType getType() const;

    // Returns the direction the NPC is facing.
    //
    // Measured in degrees around the Y axis.
    float getFacingYaw() const;

    // Returns how much the legs should swing.
    //
    // Used only for drawing the walking animation.
    float getWalkAnimationTime() const;


    // Returns true when the NPC is currently trying to walk.
    bool isMoving() const;


private:
    // --------------------------------------------------------
    // NPC identity
    // --------------------------------------------------------

    // What kind of NPC this is.
    NPCType type;


    // --------------------------------------------------------
    // Position and movement
    // --------------------------------------------------------

    // Current center position in the world.
    glm::vec3 position;


    // Position the NPC is currently trying to reach.
    glm::vec3 targetPosition;


    // Size of the NPC's invisible collision box.
    //
    // X = width
    // Y = height
    // Z = depth
    glm::vec3 size;


    // Whether the NPC currently has
    // a destination to walk toward.
    bool hasTarget;


    // Horizontal movement speed
    // in world units per second.
    float speed;

    // Direction the NPC is facing.
    //
    // Measured in degrees around the Y axis.
    float facingYaw;


    // Current movement speed on the Y axis.
    //
    // Positive = moving upward
    // Negative = falling
    // Zero     = no vertical movement
    float verticalVelocity;


    // true when the NPC is standing
    // on a solid block.
    bool grounded;

    // Position from the previous frame.
    //
    // Used to check whether the NPC
    // is actually making progress.
    glm::vec3 lastPosition;


    // Counts how long the NPC has been
    // trying to move without getting anywhere.
    float stuckTimer;


    // --------------------------------------------------------
    // Target selection
    // --------------------------------------------------------

    // Finds valid nearby blocks and randomly
    // chooses one as the next destination.
    void chooseRandomTarget(
        const World& world
    );


    // --------------------------------------------------------
    // Walking
    // --------------------------------------------------------

    // Walks toward targetPosition while
    // checking horizontal block collision.
    void moveTowardTarget(
        float deltaTime,
        const World& world
    );


    // --------------------------------------------------------
    // Collision
    // --------------------------------------------------------

    // Checks whether the NPC's collision box
    // would overlap a solid world block.
    //
    // Returns:
    // true  = collision
    // false = position is clear
    bool collidesWithWorld(
        const glm::vec3& testPosition,
        const World& world
    ) const;

    // Tracks time used for the walking animation.
    float walkAnimationTime;


    // True while the NPC is actively moving toward a target.
    bool moving;
};