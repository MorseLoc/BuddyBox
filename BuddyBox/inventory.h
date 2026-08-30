#pragma once

#include <string>
#include <vector>

#include "Block.h"
#include "Item.h"


// ============================================================
// Inventory
//
// Represents the player's hotbar.
//
// The Inventory stores:
// - Which BlockType is in each slot
// - Which slot is currently selected
//
// Inventory.cpp handles:
// - Loading the hotbar from a file
// - Cycling between slots
// - Returning selected block information
// ============================================================

// ============================================================
// Inventory slot
//
// One space in the player's inventory.
//
// Each slot remembers:
// - What item is inside
// - How many of that item the player has
// ============================================================

struct InventorySlot
{
    ItemType item;

    int amount;
};

class Inventory
{
public:
    // --------------------------------------------------------
    // Constructor
    // --------------------------------------------------------

    // Creates the player's starting hotbar.
    Inventory();


    // --------------------------------------------------------
    // Loading
    // --------------------------------------------------------

    // Loads hotbar contents from a text file.
    //
    // Returns:
    // true  = file opened successfully
    // false = file could not be opened
    bool loadFromFile(
        const std::string& filePath
    );


    // --------------------------------------------------------
    // Slot selection
    // --------------------------------------------------------

    // Moves the selected slot left or right.
    //
    // direction:
    //  1 = move forward
    // -1 = move backward
    void cycleSlot(
        int direction
    );


    // --------------------------------------------------------
    // Inventory information
    // --------------------------------------------------------

    // Returns the BlockType in the currently selected slot.
    //
    // Used when the player places a block.
    BlockType getSelectedBlockType() const;


    // Returns the BlockType stored
    // in a specific hotbar slot.
    //
    // Used by the UI when drawing the hotbar.
    BlockType getBlockTypeAtSlot(
        int slot
    ) const;

    // Returns the ItemType stored
// in a specific inventory slot.
//
// Used by the UI to draw item icons.
    ItemType getItemTypeAtSlot(
        int slot
    ) const;

    // Returns the currently selected slot number.
    //
    // Slot numbering begins at 0.
    int getSelectedSlot() const;


private:
    // --------------------------------------------------------
    // Hotbar data
    // --------------------------------------------------------

 // Stores the item and quantity
// inside each inventory slot.
    std::vector<InventorySlot> slots;


    // Index of the currently selected slot.
    int selectedSlot;
};