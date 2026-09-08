#pragma once

#include <string>
#include <vector>

#include "Block.h"
#include "Item.h"


// ============================================================
// Inventory slot
//
// Each slot stores an item type and its quantity.
// New slots start empty.
// ============================================================

struct InventorySlot
{
    ItemType item = ItemType::None;
    int amount = 0;
};


// ============================================================
// Inventory
//
// Slots 0–5:  hotbar
// Slots 6–11: storage
// ============================================================

class Inventory
{
public:
    static constexpr int HOTBAR_SLOTS = 6;
    static constexpr int SLOT_COUNT = 12;
    static constexpr int MAX_STACK_SIZE = 99;


    // --------------------------------------------------------
    // Constructor
    // --------------------------------------------------------

    Inventory();


    // --------------------------------------------------------
    // Dragging
    // --------------------------------------------------------

    // Begin dragging a nonempty slot.
    // Returns false if dragging cannot begin.
    bool beginDrag(int slot);

    // Release over a slot to move, combine, or swap items.
    void finishDrag(int destination);

    bool placeOneFromDrag(int destination);

    // Cancel without moving any items.
    void cancelDrag();

    // Returns the source slot, or -1 when nothing is dragged.
    int getDraggedSlot() const;


    // --------------------------------------------------------
    // Loading
    // --------------------------------------------------------

    // Load inventory entries from a text file.
    bool loadFromFile(
        const std::string& filePath
    );


    // --------------------------------------------------------
    // Hotbar selection
    // --------------------------------------------------------

    //  1 = next slot
    // -1 = previous slot
    void cycleSlot(
        int direction
    );


    // --------------------------------------------------------
    // Adding and removing items
    // --------------------------------------------------------

    // Fill matching stacks, then empty slots.
    // Returns false without adding anything if the full
    // amount cannot fit.
    bool addItem(
        ItemType itemType,
        int amount = 1
    );

    // Remove one item from the selected hotbar slot.
    // Returns false if that slot is empty.
    bool removeSelectedItem();


    // --------------------------------------------------------
    // Inventory information
    // --------------------------------------------------------

    // These block-type getters only describe placeable items.
    // Check the item's feature before using it to place a block.
    BlockType getSelectedBlockType() const;

    BlockType getBlockTypeAtSlot(
        int slot
    ) const;

    // Return the item type in a particular slot.
    ItemType getItemTypeAtSlot(
        int slot
    ) const;

    // Return the quantity in a particular slot.
    int getAmountAtSlot(
        int slot
    ) const;

    // Return the selected hotbar item's type.
    ItemType getSelectedItemType() const;

    // Return the selected hotbar slot number.
    int getSelectedSlot() const;


private:
    // All 12 inventory slots.
    std::vector<InventorySlot> slots;

    // Currently selected hotbar slot.
    int selectedSlot;

    // Keep dragged items in their source slot until release.
    // This makes cancelling safe even when inventory is full.
    int draggedSlot = -1;
};