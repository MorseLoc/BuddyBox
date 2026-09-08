#pragma once

#include "inventory.h"


// ============================================================
// UIRenderer
//
// Draws the hotbar, inventory, dragged items, and crosshair.
// ============================================================

class UIRenderer
{
public:
    // --------------------------------------------------------
    // Constructor and initialization
    // --------------------------------------------------------

    // Start with empty OpenGL resources.
    UIRenderer();

    // Create the rectangle, buffers, and UI shaders.
    // Returns false if initialization fails.
    bool initialize();


    // --------------------------------------------------------
    // Hotbar
    // --------------------------------------------------------

    // Draw the selected hotbar frame, item icons, and quantities.
    void drawHotbar(
        unsigned int hotbarTexture,
        unsigned int itemAtlasTexture,
        unsigned int numberAtlasTexture,
        int selectedSlot,
        const Inventory& inventory,
        int itemAtlasRows
    );


    // --------------------------------------------------------
    // Full inventory
    // --------------------------------------------------------

    // Draw all 12 slots and their contents.
    //
    // mouseX / mouseY:
    //     Mouse position converted to screen coordinates
    //     ranging from -1 to +1.
    //
    // hoveredSlot:
    //     Slot beneath the mouse, or -1 when outside.
    //
    // The dragged stack is drawn at the mouse position.
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


    // --------------------------------------------------------
    // Crosshair
    // --------------------------------------------------------

    void drawCrosshair();


private:
    // Vertex Array Object for the reusable UI rectangle.
    unsigned int VAO;

    // Vertex Buffer Object containing rectangle vertices.
    unsigned int VBO;

    // Shader program used to draw the UI.
    unsigned int shaderProgram;
};