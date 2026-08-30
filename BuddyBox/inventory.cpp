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
    // BuddyBox currently has 6 inventory slots.
    slots.resize(12);


    // Every slot starts empty.
    for (InventorySlot& slot : slots)
    {
        slot.item =
            ItemType::None;

        slot.amount =
            0;
    }


    // Slot numbers begin at 0.
    selectedSlot =
        0;
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
    std::string itemName;


    // Read one inventory slot at a time.
    while (file >> slot >> itemName)
    {
        // Ignore invalid slot numbers.
        if (
            slot < 0 ||
            slot >= static_cast<int>(slots.size())
            )
        {
            continue;
        }


        // --------------------------------------------------------
        // Convert the text name into an ItemType.
        // --------------------------------------------------------

        if (itemName == "Grass")
        {
            slots[slot].item =
                ItemType::GrassBlock;

            slots[slot].amount =
                1;
        }
        else if (itemName == "Dirt")
        {
            slots[slot].item =
                ItemType::DirtBlock;

            slots[slot].amount =
                1;
        }
        else if (itemName == "Wood")
        {
            slots[slot].item =
                ItemType::WoodBlock;

            slots[slot].amount =
                1;
        }
        else if (itemName == "Leaf")
        {
            slots[slot].item =
                ItemType::LeafBlock;

            slots[slot].amount =
                1;
        }
        else if (itemName == "Stone")
        {
            slots[slot].item =
                ItemType::StoneBlock;

            slots[slot].amount =
                1;
        }
        else if (itemName == "Stick")
        {
            slots[slot].item =
                ItemType::Stick;

            slots[slot].amount =
                1;
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
    // The hotbar is always slots 0 - 5.
//
// Slots 6 - 11 belong to the inventory
// and should not be selected by the mouse wheel.
    if (
        selectedSlot >=
        6
        )
    {
        selectedSlot = 0;
    }


    // Went before slot 0.
    // Wrap around to the final slot.
    if (selectedSlot < 0)
    {
        selectedSlot =
            5;
    }
}

// ============================================================
// Add item
//
// Adds an item to the player's inventory.
//
// First:
// Try to find an existing stack.
//
// Otherwise:
// Use the first empty slot.
//
// Returns:
// true  = item was added
// false = inventory is full
// ============================================================

bool Inventory::addItem(
    ItemType itemType,
    int amount
)
{
    // --------------------------------------------------------
    // First, look for an existing stack
    // --------------------------------------------------------

    for (InventorySlot& slot : slots)
    {
        if (
            slot.item ==
            itemType
            )
        {
            slot.amount +=
                amount;


            return true;
        }
    }


    // --------------------------------------------------------
    // Otherwise, find an empty slot
    // --------------------------------------------------------

    for (InventorySlot& slot : slots)
    {
        if (
            slot.item ==
            ItemType::None
            )
        {
            slot.item =
                itemType;


            slot.amount =
                amount;


            return true;
        }
    }


    // No existing stack and no empty slots.
    return false;
}

// ============================================================
// Inventory information
// ============================================================

// Returns the BlockType placed by the
// currently selected inventory item.
//
// This keeps the old placement system working
// while the inventory now stores ItemTypes.
BlockType Inventory::getSelectedBlockType() const
{
    Item selectedItem(
        slots[selectedSlot].item
    );


    return
        selectedItem.placedBlockType;
}


// Returns the BlockType represented by
// a specific inventory slot.
//
// This temporarily keeps the current UI working.
// Later the UI will draw directly from Itemdex.png.
BlockType Inventory::getBlockTypeAtSlot(
    int slot
) const
{
    Item item(
        slots[slot].item
    );


    return
        item.placedBlockType;
}

// Returns the ItemType stored
// in a specific inventory slot.
ItemType Inventory::getItemTypeAtSlot(
    int slot
) const
{
    return
        slots[slot].item;
}

// Returns the number of items
// stored in a specific slot.
int Inventory::getAmountAtSlot(
    int slot
) const
{
    return
        slots[slot].amount;
}

// Returns the currently selected slot number.
int Inventory::getSelectedSlot() const
{
    return selectedSlot;
}

// Returns the ItemType currently
// selected by the player.
ItemType Inventory::getSelectedItemType() const
{
    return
        slots[selectedSlot].item;
}