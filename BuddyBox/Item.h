#pragma once

#include "Block.h"


// ============================================================
// Item types
//
// Every thing that can exist in the player's inventory
// gets an ItemType.
//
// None means "no item".
// ============================================================

enum class ItemType
{
    None,

    GrassBlock,
    DirtBlock,
    WoodBlock,
    LeafBlock,
    StoneBlock,

    Stick
};


// ============================================================
// Item features
//
// Describes what an item can currently do.
//
// We can add more later:
// - Tool
// - Food
// - Weapon
// - Structure
// ============================================================

enum class ItemFeature
{
    None,

    PlaceBlock
};


// ============================================================
// Item
//
// Stores the permanent properties of one item type.
// ============================================================

struct Item
{
    // What item this is.
    ItemType type;


    // What kind of behavior this item has.
    ItemFeature feature;


    // Which row of Itemdex.png this item uses.
    int textureRow;


    // The block this item places.
    //
    // Only used when feature == PlaceBlock.
    BlockType placedBlockType;


    // Creates an item and gives it the
    // correct properties for its ItemType.
    Item(
        ItemType itemType
    );
};