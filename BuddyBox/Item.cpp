#include "Item.h"


// ============================================================
// Item constructor
// ============================================================

Item::Item(
    ItemType itemType
)
{
    type = itemType;

    feature = ItemFeature::None;

    textureRow = -1;

    placedBlockType = BlockType::Grass;

    // --------------------------------------------------------
    // Placeable block items
    // --------------------------------------------------------

    if (type == ItemType::GrassBlock)
    {
        textureRow = 0;
        feature = ItemFeature::PlaceBlock;
        placedBlockType = BlockType::Grass;
    }
    else if (type == ItemType::DirtBlock)
    {
        textureRow = 1;
        feature = ItemFeature::PlaceBlock;
        placedBlockType = BlockType::Dirt;
    }
    else if (type == ItemType::WoodBlock)
    {
        textureRow = 2;
        feature = ItemFeature::PlaceBlock;
        placedBlockType = BlockType::Wood;
    }
    else if (type == ItemType::Wood2Block)
    {
        textureRow = 5;
        feature = ItemFeature::PlaceBlock;
        placedBlockType = BlockType::Wood2;
    }
    else if (type == ItemType::Wood3Block)
    {
        textureRow = 6;
        feature = ItemFeature::PlaceBlock;
        placedBlockType = BlockType::Wood3;
    }
    else if (type == ItemType::Wood4Block)
    {
        textureRow = 7;
        feature = ItemFeature::PlaceBlock;
        placedBlockType = BlockType::Wood4;
    }
    else if (type == ItemType::Bulb)
    {
        textureRow = 8;
        feature = ItemFeature::PlaceBlock;
        placedBlockType = BlockType::Bulb;
    }
    else if (type == ItemType::Sapling)
    {
        // Temporary art: reuse the Stick item icon.
        textureRow = 3;
        feature = ItemFeature::PlaceBlock;
        placedBlockType = BlockType::Sapling;
    }

    // --------------------------------------------------------
    // Non-placeable items
    // --------------------------------------------------------

    else if (type == ItemType::Stick)
    {
        textureRow = 3;
    }
    else if (type == ItemType::Pebble)
    {
        textureRow = 4;
    }
}