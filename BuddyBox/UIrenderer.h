#pragma once

#include "inventory.h"

// Draws the hotbar, inventory, crosshair, and animated hand.
class UIRenderer
{
public:
    UIRenderer();

    bool initialize();

    // Hotbar
    void drawHotbar(
        unsigned int hotbarTexture,
        unsigned int itemAtlasTexture,
        unsigned int numberAtlasTexture,
        int selectedSlot,
        const Inventory& inventory,
        int itemAtlasRows
    );

    // Full inventory
    void drawInventory(
        unsigned int inventoryTexture,
        unsigned int itemAtlasTexture,
        unsigned int numberAtlasTexture,
        const Inventory& inventory,
        int itemAtlasRows,
        float mouseX,
        float mouseY,
        int hoveredSlot
    );

    // Crosshair
    void drawCrosshair();

    // Animated hand and held item
    void drawHand(
        unsigned int handTexture,
        unsigned int itemAtlasTexture,
        ItemType heldItem,
        int itemAtlasRows,
        int framebufferWidth,
        int framebufferHeight
    );

    void drawHealth(
        unsigned int healthTexture,
        int health,
        int maximumHealth
    );

private:
    unsigned int VAO;
    unsigned int VBO;
    unsigned int shaderProgram;
};