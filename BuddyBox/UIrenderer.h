#pragma once

#include "inventory.h"


// ============================================================
// UIRenderer
//
// Handles BuddyBox's 2D user interface.
//
// The UIRenderer currently draws:
// - The player's hotbar
// - Block icons inside the hotbar
// - The crosshair
//
// UIRenderer.cpp contains the OpenGL setup,
// shaders, positioning, and drawing logic.
// ============================================================

class UIRenderer
{
public:
    // --------------------------------------------------------
    // Constructor
    // --------------------------------------------------------

    // Creates an empty UI renderer.
    //
    // initialize() creates the OpenGL resources.
    UIRenderer();


    // --------------------------------------------------------
    // Initialization
    // --------------------------------------------------------

    // Creates everything needed to draw 2D UI.
    //
    // This includes:
    // - Rectangle vertex data
    // - VAO and VBO
    // - UI shaders
    //
    // Returns true when initialization finishes.
    bool initialize();


    // --------------------------------------------------------
    // Hotbar
    // --------------------------------------------------------

    // Draws the player's hotbar and block icons.
    //
    // hotbarTexture:
    //     Texture containing the hotbar frames.
    //
    // blockAtlasTexture:
    //     Main block texture atlas used for the icons.
    //
    // selectedSlot:
    //     Currently selected hotbar slot.
    //
    // inventory:
    //     Provides the BlockType stored in each slot.
    //
    // atlasRows:
    //     Number of block texture rows in the atlas.
    void drawHotbar(
        unsigned int hotbarTexture,
        unsigned int itemAtlasTexture,
        unsigned int numberAtlasTexture,
        int selectedSlot,
        const Inventory& inventory,
        int itemAtlasRows
    );


    // --------------------------------------------------------
    // Crosshair
    // --------------------------------------------------------

    // Draws the white crosshair
    // in the center of the screen.
    void drawCrosshair();


private:
    // --------------------------------------------------------
    // OpenGL resources
    // --------------------------------------------------------

    // Vertex Array Object for the reusable UI rectangle.
    unsigned int VAO;


    // Vertex Buffer Object containing
    // the rectangle's vertex data.
    unsigned int VBO;


    // Shader program used to draw the UI.
    unsigned int shaderProgram;
};