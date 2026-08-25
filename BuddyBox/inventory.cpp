#include "inventory.h"


Inventory::Inventory()
{
	// Fill the six hotbar slots
	// with the block types we currently have.
	slots =
	{
		BlockType::Grass,
		BlockType::Spawner,
		BlockType::Dirt,
		BlockType::Wood,
		BlockType::Leaf,
		BlockType::Stone
	};

	// Start on the first slot.
	selectedSlot = 0;
}


void Inventory::cycleSlot(int direction)
{
	selectedSlot += direction;

	// If we go past the last slot,
	// wrap back to the first slot.
	if (selectedSlot >= static_cast<int>(slots.size()))
	{
		selectedSlot = 0;
	}

	// If we go before the first slot,
	// wrap around to the last slot.
	if (selectedSlot < 0)
	{
		selectedSlot =
			static_cast<int>(slots.size()) - 1;
	}
}


BlockType Inventory::getSelectedBlockType() const
{
	return slots[selectedSlot];
}


int Inventory::getSelectedSlot() const
{
	return selectedSlot;
}