#include "Item.h"


// ============================================================
// Item constructor
//
// Gives each ItemType its permanent properties.
// ============================================================

Item::Item(
    ItemType itemType
)
{
    type =
        itemType;


    // --------------------------------------------------------
    // Default item properties
    // --------------------------------------------------------

    feature =
        ItemFeature::None;


    textureRow =
        -1;


    // This value does not matter unless the item
    // actually has PlaceBlock behavior.
    placedBlockType =
        BlockType::Grass;


    // --------------------------------------------------------
    // Block items
    // --------------------------------------------------------

    if (type == ItemType::GrassBlock)
    {
        textureRow =
            0;

        feature =
            ItemFeature::PlaceBlock;

        placedBlockType =
            BlockType::Grass;
    }
    else if (type == ItemType::DirtBlock)
    {
        textureRow =
            1;

        feature =
            ItemFeature::PlaceBlock;

        placedBlockType =
            BlockType::Dirt;
    }
    else if (type == ItemType::WoodBlock)
    {
        textureRow =
            2;

        feature =
            ItemFeature::PlaceBlock;

        placedBlockType =
            BlockType::Wood;
    }
    else if (type == ItemType::Wood2Block)
    {
        textureRow =
            5;

        feature =
            ItemFeature::PlaceBlock;

        placedBlockType =
            BlockType::Wood2;
    }
    else if (type == ItemType::Wood3Block)
    {
        textureRow =
            6;

        feature =
            ItemFeature::PlaceBlock;

        placedBlockType =
            BlockType::Wood3;
    }
    else if (type == ItemType::Wood4Block)
    {
        textureRow =
            7;

        feature =
            ItemFeature::PlaceBlock;

        placedBlockType =
            BlockType::Wood4;
    }


    // --------------------------------------------------------
    // Crafting items
    // --------------------------------------------------------

    else if (type == ItemType::Stick)
    {
        textureRow =
            3;

        feature =
            ItemFeature::None;
    }
    else if (type == ItemType::Pebble)
    {
        textureRow =
            4;

        feature =
            ItemFeature::None;
}