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
    else if (type == ItemType::LeafBlock)
    {
        textureRow =
            3;

        feature =
            ItemFeature::PlaceBlock;

        placedBlockType =
            BlockType::Leaf;
    }
    else if (type == ItemType::StoneBlock)
    {
        textureRow =
            4;

        feature =
            ItemFeature::PlaceBlock;

        placedBlockType =
            BlockType::Stone;
    }


    // --------------------------------------------------------
    // Crafting items
    // --------------------------------------------------------

    else if (type == ItemType::Stick)
    {
        textureRow =
            5;

        feature =
            ItemFeature::None;
    }
}