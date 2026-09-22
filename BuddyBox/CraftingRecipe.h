#pragma once

#include <array>
#include "Item.h"

enum class RecipeLayout
{
    Shaped,
    Shapeless
};

struct CraftingRecipe
{
    RecipeLayout layout = RecipeLayout::Shaped;

    // Grid order:
    // 0 = top left,    1 = top right
    // 2 = bottom left, 3 = bottom right
    //
    // Each ingredient uses one item from its stack.
    // None represents an empty slot.
    std::array<ItemType, 4> ingredients =
    {
        ItemType::None,
        ItemType::None,
        ItemType::None,
        ItemType::None
    };

    ItemType output = ItemType::None;
    int outputAmount = 1;
};