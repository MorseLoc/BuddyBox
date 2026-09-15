#pragma once

#include "Block.h"


// ============================================================
// Item types
// ============================================================

enum class ItemType
{
    None,

    GrassBlock,
    DirtBlock,
    WoodBlock,
    Stick,
    Pebble,
    Wood2Block,
    Wood3Block,
    Wood4Block,
    Bulb,
    Sapling
};


// ============================================================
// Item features
// ============================================================

enum class ItemFeature
{
    None,

    PlaceBlock
};


// ============================================================
// Item
// ============================================================

struct Item
{
    // What kind of item this is.
    ItemType type;

    // What the item can do.
    ItemFeature feature;

    // Row in Itemdex.png.
    int textureRow;

    // Block placed when this is a placeable item.
    BlockType placedBlockType;

    Item(
        ItemType itemType
    );
};