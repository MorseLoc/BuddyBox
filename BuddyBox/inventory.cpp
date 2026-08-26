#include "inventory.h"

#include <fstream>
#include <iostream>
#include <string>


Inventory::Inventory()
{
	// Create six inventory slots.
	slots.resize(6);

	// Start with slot 0 selected.
	selectedSlot = 0;
}

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

	while (file >> slot >> blockName)
	{
		if (slot < 0 || slot >= 6)
		{
			continue;
		}

		if (blockName == "Grass")
		{
			slots[slot] = BlockType::Grass;
		}
		else if (blockName == "Spawner")
		{
			slots[slot] = BlockType::Spawner;
		}
		else if (blockName == "Dirt")
		{
			slots[slot] = BlockType::Dirt;
		}
		else if (blockName == "Wood")
		{
			slots[slot] = BlockType::Wood;
		}
		else if (blockName == "Leaf")
		{
			slots[slot] = BlockType::Leaf;
		}
		else if (blockName == "Stone")
		{
			slots[slot] = BlockType::Stone;
		}
	}

	std::cout
		<< "Inventory loaded.\n";

	return true;
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

BlockType Inventory::getBlockTypeAtSlot(
	int slot
) const
{
	return slots[slot];
}


int Inventory::getSelectedSlot() const
{
	return selectedSlot;
}