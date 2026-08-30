#include "inventory.h"

#include <fstream>
#include <iostream>
#include <string>


// ============================================================
// Inventory constructor
//
// Creates the player's inventory.
//
// Slots:
// 0 - 5  = hotbar
// 6 - 11 = inventory storage
// ============================================================

Inventory::Inventory()
{
    // BuddyBox currently has:
    //
    // 6 hotbar slots
    // 6 inventory slots
    //
    // 12 total.
    slots.resize(
        12
    );


    // Every slot starts empty.
    for (InventorySlot& slot : slots)
    {
        slot.item =
            ItemType::None;


        slot.amount =
            0;
    }


    // Hotbar slot numbers begin at 0.
    selectedSlot =
        0;
}


// ============================================================
// Load inventory
//
// Loads inventory contents from inventory.txt.
//
// Expected format:
//
// SlotNumber ItemName
//
// Example:
//
// 0 Grass
// 1 Dirt
// 2 Stone
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


    // Read one inventory entry at a time.
    while (
        file >>
        slot >>
        itemName
        )
    {
        // Ignore invalid slot numbers.
        if (
            slot < 0 ||
            slot >=
            static_cast<int>(
                slots.size()
                )
            )
        {
            continue;
        }


        // --------------------------------------------------------
        // Convert text item names into ItemTypes
        // --------------------------------------------------------

        if (
            itemName ==
            "Grass"
            )
        {
            slots[slot].item =
                ItemType::GrassBlock;


            slots[slot].amount =
                1;
        }

        else if (
            itemName ==
            "Dirt"
            )
        {
            slots[slot].item =
                ItemType::DirtBlock;


            slots[slot].amount =
                1;
        }

        else if (
            itemName ==
            "Wood"
            )
        {
            slots[slot].item =
                ItemType::WoodBlock;


            slots[slot].amount =
                1;
        }

        else if (
            itemName ==
            "Leaf"
            )
        {
            slots[slot].item =
                ItemType::LeafBlock;


            slots[slot].amount =
                1;
        }

        else if (
            itemName ==
            "Stone"
            )
        {
            slots[slot].item =
                ItemType::StoneBlock;


            slots[slot].amount =
                1;
        }

        else if (
            itemName ==
            "Stick"
            )
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
// Moves the selected hotbar slot.
//
// direction:
//
//  1 = next slot
// -1 = previous slot
//
// Only slots 0 - 5 are selectable
// using the mouse wheel.
// ============================================================

void Inventory::cycleSlot(
    int direction
)
{
    selectedSlot +=
        direction;


    // Went past slot 5.
    if (
        selectedSlot >=
        6
        )
    {
        selectedSlot =
            0;
    }


    // Went before slot 0.
    if (
        selectedSlot <
        0
        )
    {
        selectedSlot =
            5;
    }
}


// ============================================================
// Add item
//
// Adds items to the inventory.
//
// Maximum stack size:
// 99
//
// First:
// Fill existing stacks.
//
// Then:
// Create new stacks in empty slots.
//
// Example:
//
// Existing:
//
// Stick x99
//
// Add 1 stick:
//
// Stick x99
// Stick x1
//
// Returns:
//
// true  = all items were added
// false = there was not enough inventory space
// ============================================================

bool Inventory::addItem(
    ItemType itemType,
    int amount
)
{
    const int maxStackSize =
        99;


    // --------------------------------------------------------
    // Reject invalid additions
    // --------------------------------------------------------

    if (
        itemType ==
        ItemType::None ||
        amount <=
        0
        )
    {
        return false;
    }


    // ========================================================
    // Check whether the ENTIRE amount can fit
    // ========================================================

    int availableSpace =
        0;


    for (const InventorySlot& slot : slots)
    {
        // Existing stack of the same item.
        if (
            slot.item ==
            itemType
            )
        {
            availableSpace +=
                maxStackSize -
                slot.amount;
        }

        // Empty slot can hold a fresh stack.
        else if (
            slot.item ==
            ItemType::None
            )
        {
            availableSpace +=
                maxStackSize;
        }
    }


    // Don't partially pick something up.
    //
    // Either the whole amount fits,
    // or nothing is changed.
    if (
        availableSpace <
        amount
        )
    {
        return false;
    }


    int remainingAmount =
        amount;


    // ========================================================
    // Fill existing stacks first
    // ========================================================

    for (InventorySlot& slot : slots)
    {
        if (
            slot.item ==
            itemType &&
            slot.amount <
            maxStackSize
            )
        {
            int spaceInStack =
                maxStackSize -
                slot.amount;


            int amountToAdd;


            if (
                remainingAmount <
                spaceInStack
                )
            {
                amountToAdd =
                    remainingAmount;
            }
            else
            {
                amountToAdd =
                    spaceInStack;
            }


            slot.amount +=
                amountToAdd;


            remainingAmount -=
                amountToAdd;


            // Everything has been added.
            if (
                remainingAmount ==
                0
                )
            {
                return true;
            }
        }
    }


    // ========================================================
    // Create new stacks
    // ========================================================

    for (InventorySlot& slot : slots)
    {
        if (
            slot.item ==
            ItemType::None
            )
        {
            int amountToAdd;


            if (
                remainingAmount <
                maxStackSize
                )
            {
                amountToAdd =
                    remainingAmount;
            }
            else
            {
                amountToAdd =
                    maxStackSize;
            }


            slot.item =
                itemType;


            slot.amount =
                amountToAdd;


            remainingAmount -=
                amountToAdd;


            // Everything has been added.
            if (
                remainingAmount ==
                0
                )
            {
                return true;
            }
        }
    }


    // This should normally never happen
    // because we checked available space first.
    return false;
}


// ============================================================
// Remove selected item
//
// Removes ONE item from the currently selected
// hotbar slot.
//
// Used after successfully placing a block.
//
// Example:
//
// Dirt x3
//
// Place block:
//
// Dirt x2
//
// When the amount reaches zero,
// the slot becomes completely empty.
// ============================================================

bool Inventory::removeSelectedItem()
{
    InventorySlot& slot =
        slots[selectedSlot];


    // Nothing exists in this slot.
    if (
        slot.item ==
        ItemType::None ||
        slot.amount <=
        0
        )
    {
        return false;
    }


    // Remove one item.
    slot.amount -=
        1;


    // --------------------------------------------------------
    // Stack became empty
    // --------------------------------------------------------

    if (
        slot.amount <=
        0
        )
    {
        slot.amount =
            0;


        slot.item =
            ItemType::None;
    }


    return true;
}


// ============================================================
// Inventory information
// ============================================================


// ------------------------------------------------------------
// Get selected block type
//
// Returns the block created by
// the currently selected item.
// ------------------------------------------------------------

BlockType Inventory::getSelectedBlockType() const
{
    Item selectedItem(
        slots[selectedSlot].item
    );


    return
        selectedItem.placedBlockType;
}


// ------------------------------------------------------------
// Get block type at slot
//
// Returns the block represented by
// the item inside a specific slot.
// ------------------------------------------------------------

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


// ------------------------------------------------------------
// Get item type at slot
// ------------------------------------------------------------

ItemType Inventory::getItemTypeAtSlot(
    int slot
) const
{
    return
        slots[slot].item;
}


// ------------------------------------------------------------
// Get amount at slot
// ------------------------------------------------------------

int Inventory::getAmountAtSlot(
    int slot
) const
{
    return
        slots[slot].amount;
}


// ------------------------------------------------------------
// Get selected item type
// ------------------------------------------------------------

ItemType Inventory::getSelectedItemType() const
{
    return
        slots[selectedSlot].item;
}


// ------------------------------------------------------------
// Get selected slot
// ------------------------------------------------------------

int Inventory::getSelectedSlot() const
{
    return
        selectedSlot;
}