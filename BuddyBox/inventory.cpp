#include "inventory.h"

#include <fstream>
#include <iostream>
#include <string>


// ============================================================
// Inventory constructor
//
// Creates the player's hotbar and selects the first slot.
// ============================================================

Inventory::Inventory()
{
    // BuddyBox currently has 6 hotbar slots.
    slots.resize(6);


    // Slot numbers begin at 0.
    selectedSlot = 0;
}


// ============================================================
// Load inventory
//
// Loads hotbar contents from a text file.
//
// Expected format:
//
// SlotNumber BlockType
//
// Example:
//
// 0 Grass
// 1 Dirt
// 2 Stone
//
// Returns:
// true  = file opened successfully
// false = file could not be opened
// ============================================================

bool Inventory::loadFromFile(
    const std::string& filePath
)
{
    std::ifstream file(
        filePath
    );


    if (!file.is_open())
    {
        std::cout
            << "Failed to load inventory: "
            << filePath
            << "\n";


        return false;
    }


    int slot;
    std::string blockName;


    // Read one hotbar slot at a time.
    while (file >> slot >> blockName)
    {
        // Ignore invalid slot numbers.
        if (
            slot < 0 ||
            slot >= static_cast<int>(slots.size())
            )
        {
            continue;
        }


        // Convert the block's text name
        // into a BlockType.
        if (blockName == "Grass")
        {
            slots[slot] =
                BlockType::Grass;
        }
        else if (blockName == "Spawner")
        {
            slots[slot] =
                BlockType::Spawner;
        }
        else if (blockName == "Dirt")
        {
            slots[slot] =
                BlockType::Dirt;
        }
        else if (blockName == "Wood")
        {
            slots[slot] =
                BlockType::Wood;
        }
        else if (blockName == "Leaf")
        {
            slots[slot] =
                BlockType::Leaf;
        }
        else if (blockName == "Stone")
        {
            slots[slot] =
                BlockType::Stone;
        }
    }


    std::cout
        << "Inventory loaded.\n";


    return true;
}


// ============================================================
// Cycle selected slot
//
// Moves the selected hotbar slot left or right.
//
// direction:
//  1 = move forward
// -1 = move backward
//
// The selection wraps around when it reaches either end.
// ============================================================

void Inventory::cycleSlot(
    int direction
)
{
    selectedSlot +=
        direction;


    // Went past the final slot.
    // Wrap around to the beginning.
    if (
        selectedSlot >=
        static_cast<int>(slots.size())
        )
    {
        selectedSlot = 0;
    }


    // Went before slot 0.
    // Wrap around to the final slot.
    if (selectedSlot < 0)
    {
        selectedSlot =
            static_cast<int>(slots.size()) - 1;
    }
}


// ============================================================
// Inventory information
// ============================================================

// Returns the BlockType currently selected by the player.
//
// Used when placing blocks.
BlockType Inventory::getSelectedBlockType() const
{
    return slots[selectedSlot];
}


// Returns the BlockType stored in a specific hotbar slot.
//
// Used by the UI when drawing each slot.
BlockType Inventory::getBlockTypeAtSlot(
    int slot
) const
{
    return slots[slot];
}


// Returns the currently selected slot number.
int Inventory::getSelectedSlot() const
{
    return selectedSlot;
}