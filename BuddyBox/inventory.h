#pragma once

#include "Block.h"

#include <vector>

// Stores the player's hotbar.
class Inventory
{
public:

	// Creates the six starting hotbar slots.
	Inventory();

	// Moves the selected slot left or right.
	void cycleSlot(int direction);

	// Returns the currently selected block type.
	BlockType getSelectedBlockType() const;

	// Returns the selected slot number.
	// 0 = first slot
	// 5 = sixth slot
	int getSelectedSlot() const;


private:

	// The six block types stored in the hotbar.
	std::vector<BlockType> slots;

	// Which hotbar slot is currently selected.
	int selectedSlot;
};