#include "inventory.h"

#include <algorithm>
#include <utility>
#include <fstream>
#include <iostream>
#include <string>


// ============================================================
// Constructor
// ============================================================

Inventory::Inventory()
{
    slots.resize(SLOT_COUNT);

    for (InventorySlot& slot : slots)
    {
        slot.item = ItemType::None;
        slot.amount = 0;
    }

    selectedSlot = 0;
}


// ============================================================
// Load inventory
//
// Format: SlotNumber ItemName
//
// Example:
// 0 Grass
// 1 Stick
// ============================================================

bool Inventory::loadFromFile(
    const std::string& filePath
)
{
    std::ifstream file(filePath);

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

    while (file >> slot >> itemName)
    {
        // Ignore slot numbers outside the inventory.
        if (slot < 0 || slot >= static_cast<int>(slots.size()))
        {
            continue;
        }

        if (itemName == "Grass")
        {
            slots[slot].item = ItemType::GrassBlock;
            slots[slot].amount = 1;
        }
        else if (itemName == "Dirt")
        {
            slots[slot].item = ItemType::DirtBlock;
            slots[slot].amount = 1;
        }
        else if (itemName == "Wood")
        {
            slots[slot].item = ItemType::WoodBlock;
            slots[slot].amount = 1;
        }
        else if (itemName == "Wood2")
        {
            slots[slot].item = ItemType::Wood2Block;
            slots[slot].amount = 1;
        }
        else if (itemName == "Wood3")
        {
            slots[slot].item = ItemType::Wood3Block;
            slots[slot].amount = 1;
        }
        else if (itemName == "Wood4")
        {
            slots[slot].item = ItemType::Wood4Block;
            slots[slot].amount = 1;
        }
        else if (itemName == "Pebble")
        {
            slots[slot].item = ItemType::Pebble;
            slots[slot].amount = 1;
        }
        else if (itemName == "Stick")
        {
            slots[slot].item = ItemType::Stick;
            slots[slot].amount = 1;
        }
        else if (itemName == "Bulb")
        {
            slots[slot].item = ItemType::Bulb;
            slots[slot].amount = 1;
        }
    }

    std::cout << "Inventory loaded.\n";

    return true;
}

bool Inventory::placeOneFromDrag(int destination)
{
    if (
        draggedSlot < 0 ||
        destination < 0 ||
        destination >= SLOT_COUNT ||
        destination == draggedSlot
        )
    {
        return false;
    }

    InventorySlot& from = slots[draggedSlot];
    InventorySlot& to = slots[destination];

    if (from.item == ItemType::None || from.amount <= 0)
    {
        cancelDrag();
        return false;
    }

    if (to.item == ItemType::None)
    {
        to.item = from.item;
        to.amount = 1;
    }
    else if (
        to.item == from.item &&
        to.amount < MAX_STACK_SIZE
        )
    {
        to.amount += 1;
    }
    else
    {
        return false;
    }

    from.amount -= 1;

    if (from.amount == 0)
    {
        from = InventorySlot{};
        cancelDrag();
    }

    return true;
}

// ============================================================
// Cycle selected hotbar slot
// ============================================================

void Inventory::cycleSlot(int direction)
{
    selectedSlot += direction;

    if (selectedSlot >= 6)
    {
        selectedSlot = 0;
    }

    if (selectedSlot < 0)
    {
        selectedSlot = 5;
    }
}


// ============================================================
// Add items
//
// Check space first so pickups are all-or-nothing.
// Keep the stack being dragged reserved from automatic pickups.
// ============================================================

bool Inventory::addItem(
    ItemType itemType,
    int amount
)
{
    const int maxStackSize = MAX_STACK_SIZE;

    if (itemType == ItemType::None || amount <= 0)
    {
        return false;
    }

    // Count available space.
    int availableSpace = 0;

    for (const InventorySlot& slot : slots)
    {
        if (draggedSlot >= 0 && &slot == &slots[draggedSlot])
        {
            continue;
        }

        if (slot.item == itemType)
        {
            availableSpace += maxStackSize - slot.amount;
        }
        else if (slot.item == ItemType::None)
        {
            availableSpace += maxStackSize;
        }
    }

    // Leave the inventory unchanged if everything cannot fit.
    if (availableSpace < amount)
    {
        return false;
    }

    int remainingAmount = amount;

    // Fill existing stacks first.
    for (InventorySlot& slot : slots)
    {
        if (draggedSlot >= 0 && &slot == &slots[draggedSlot])
        {
            continue;
        }

        if (slot.item == itemType && slot.amount < maxStackSize)
        {
            int spaceInStack = maxStackSize - slot.amount;
            int amountToAdd;

            if (remainingAmount < spaceInStack)
            {
                amountToAdd = remainingAmount;
            }
            else
            {
                amountToAdd = spaceInStack;
            }

            slot.amount += amountToAdd;
            remainingAmount -= amountToAdd;

            if (remainingAmount == 0)
            {
                return true;
            }
        }
    }

    // Put remaining items into empty slots.
    for (InventorySlot& slot : slots)
    {
        if (draggedSlot >= 0 && &slot == &slots[draggedSlot])
        {
            continue;
        }

        if (slot.item == ItemType::None)
        {
            int amountToAdd;

            if (remainingAmount < maxStackSize)
            {
                amountToAdd = remainingAmount;
            }
            else
            {
                amountToAdd = maxStackSize;
            }

            slot.item = itemType;
            slot.amount = amountToAdd;
            remainingAmount -= amountToAdd;

            if (remainingAmount == 0)
            {
                return true;
            }
        }
    }

    return false;
}


// ============================================================
// Remove one item from the selected hotbar slot
// ============================================================

bool Inventory::removeSelectedItem()
{
    InventorySlot& slot = slots[selectedSlot];

    if (slot.item == ItemType::None || slot.amount <= 0)
    {
        return false;
    }

    slot.amount -= 1;

    if (slot.amount <= 0)
    {
        slot.amount = 0;
        slot.item = ItemType::None;
    }

    return true;
}

std::vector<InventorySlot> Inventory::takeAll()
{
    cancelDrag();

    std::vector<InventorySlot> removed;

    for (InventorySlot& slot : slots)
    {
        if (slot.item != ItemType::None && slot.amount > 0)
        {
            removed.push_back(slot);
        }

        slot = InventorySlot{};
    }

    return removed;
}

// ============================================================
// Inventory information
// ============================================================

BlockType Inventory::getSelectedBlockType() const
{
    Item selectedItem(slots[selectedSlot].item);

    return selectedItem.placedBlockType;
}


BlockType Inventory::getBlockTypeAtSlot(int slot) const
{
    Item item(slots[slot].item);

    return item.placedBlockType;
}


ItemType Inventory::getItemTypeAtSlot(int slot) const
{
    return slots[slot].item;
}


int Inventory::getAmountAtSlot(int slot) const
{
    return slots[slot].amount;
}


ItemType Inventory::getSelectedItemType() const
{
    return slots[selectedSlot].item;
}


int Inventory::getSelectedSlot() const
{
    return selectedSlot;
}


// ============================================================
// Begin dragging
//
// Items stay in their source slot until the mouse is released.
// ============================================================

bool Inventory::beginDrag(int slot)
{
    if (draggedSlot != -1 || slot < 0 || slot >= SLOT_COUNT ||
        slots[slot].item == ItemType::None || slots[slot].amount <= 0)
    {
        return false;
    }

    draggedSlot = slot;

    return true;
}


// ============================================================
// Finish dragging
//
// Empty destination: move.
// Same item: combine, leaving overflow in the original slot.
// Different item: swap.
// Invalid destination: cancel.
// ============================================================

void Inventory::finishDrag(int destination)
{
    const int source = draggedSlot;

    cancelDrag();

    if (source < 0 || destination < 0 || destination >= SLOT_COUNT ||
        destination == source)
    {
        return;
    }

    InventorySlot& from = slots[source];
    InventorySlot& to = slots[destination];

    if (from.item == to.item)
    {
        const int moved = std::min(
            from.amount,
            MAX_STACK_SIZE - to.amount
        );

        to.amount += moved;
        from.amount -= moved;

        if (from.amount == 0)
        {
            from = InventorySlot{};
        }
    }
    else
    {
        // Swapping with an empty slot also handles a normal move.
        std::swap(from, to);
    }
}


// ============================================================
// Cancel dragging
// ============================================================

void Inventory::cancelDrag()
{
    draggedSlot = -1;
}


// ============================================================
// Get the slot currently being dragged
// ============================================================

int Inventory::getDraggedSlot() const
{
    return draggedSlot;
}