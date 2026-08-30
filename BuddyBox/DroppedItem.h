#pragma once

#include <glm/glm.hpp>

#include "Item.h"


// ============================================================
// Dropped item
//
// Represents an item physically sitting in the world.
//
// Later this will handle:
// - Falling
// - Block collision
// - Pickup by the player
//
// Rendering will stay separate.
// ============================================================

struct DroppedItem
{
    // What item this is.
    ItemType type;


    // Current center position in the world.
    glm::vec3 position;


    // Current vertical movement speed.
    //
    // Negative = falling.
    // Positive = moving upward.
    float verticalVelocity;


    // Creates a dropped item at a world position.
    DroppedItem(
        ItemType itemType,
        const glm::vec3& startPosition
    )
        :
        type(itemType),
        position(startPosition),
        verticalVelocity(0.0f)
    {
    }
};