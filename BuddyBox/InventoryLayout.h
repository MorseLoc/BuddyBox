#pragma once

#include <cmath>
#include "inventory.h"

// Shared by drawing and mouse hit testing so their slots cannot drift.
// Coordinates are OpenGL screen coordinates: -1 to +1.
namespace InventoryLayout
{
    inline constexpr float LEFT = -0.45f;
    inline constexpr float BOTTOM = -0.925f;
    inline constexpr float CELL_SIZE = 0.15f;
    inline constexpr float HALF_WIDTH = 0.45f;
    inline constexpr float HALF_HEIGHT = 0.15f;
    inline constexpr float CENTER_Y = -0.775f;

    inline float slotX(int slot)
    {
        return LEFT + (slot % Inventory::HOTBAR_SLOTS + 0.5f) * CELL_SIZE;
    }

    inline float slotY(int slot)
    {
        return BOTTOM + (slot / Inventory::HOTBAR_SLOTS + 0.5f) * CELL_SIZE;
    }

    // GLFW cursor positions and window sizes both use screen coordinates,
    // unlike framebuffer pixels. This also works with Windows scaling.
    inline int slotAt(double mouseX, double mouseY, int width, int height)
    {
        if (width <= 0 || height <= 0 || !std::isfinite(mouseX) ||
            !std::isfinite(mouseY)) return -1;

        const double x = 2.0 * mouseX / width - 1.0;
        const double y = 1.0 - 2.0 * mouseY / height;

        const double column = (x - LEFT) / CELL_SIZE;
        const double row = (y - BOTTOM) / CELL_SIZE;

        if (column < 0.0 || column >= Inventory::HOTBAR_SLOTS ||
            row < 0.0 || row >= 2.0) return -1;

        return static_cast<int>(row) * Inventory::HOTBAR_SLOTS +
            static_cast<int>(column);
    }
}