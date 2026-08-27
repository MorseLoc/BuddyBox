#pragma once

#include <string>


// ============================================================
// TextureManager
//
// Handles loading textures used by BuddyBox.
//
// It is responsible for:
// - Loading the block texture atlas
// - Loading standalone textures
// - Remembering the atlas size
// - Reporting how many block rows exist in the atlas
// ============================================================

class TextureManager
{
public:
    // --------------------------------------------------------
    // Constructor
    // --------------------------------------------------------

    // Creates an empty texture manager.
    TextureManager();


    // --------------------------------------------------------
    // Texture loading
    // --------------------------------------------------------

    // Loads the block texture atlas.
    //
    // Returns:
    // true  = atlas loaded successfully
    // false = loading failed
    bool loadAtlas(
        const std::string& texturePath
    );


    // Loads a normal standalone 2D texture.
    //
    // Returns the OpenGL texture ID.
    //
    // Returns 0 if loading fails.
    unsigned int loadTexture(
        const std::string& texturePath
    );


    // --------------------------------------------------------
    // Atlas information
    // --------------------------------------------------------

    // Returns the OpenGL texture ID
    // belonging to the block atlas.
    unsigned int getAtlasTexture() const;


    // Returns how many block texture rows
    // currently exist in the atlas.
    int getBlockCount() const;


private:
    // --------------------------------------------------------
    // Atlas data
    // --------------------------------------------------------

    // OpenGL texture ID for the block atlas.
    //
    // 0 means no atlas is currently loaded.
    unsigned int atlasTexture;


    // Width and height of the loaded atlas in pixels.
    int atlasWidth;
    int atlasHeight;


    // Width and height of one BuddyBox sprite.
    //
    // Current block textures are 16 x 16 pixels.
    static const int tileSize = 16;
};