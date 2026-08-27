#include "TextureManager.h"

#include <glad/glad.h>
#include <stb_image.h>

#include <iostream>


// ============================================================
// TextureManager constructor
//
// Starts with no texture atlas loaded.
// ============================================================

TextureManager::TextureManager()
{
    // OpenGL texture ID for the block atlas.
    //
    // 0 means no texture has been created yet.
    atlasTexture = 0;


    // Atlas dimensions are unknown until loadAtlas() succeeds.
    atlasWidth = 0;
    atlasHeight = 0;
}


// ============================================================
// Load block texture atlas
//
// Loads the image that contains all BuddyBox block textures
// and uploads it to the GPU.
//
// Returns:
// true  = texture loaded successfully
// false = image could not be loaded
// ============================================================

bool TextureManager::loadAtlas(
    const std::string& texturePath
)
{
    int channels;


    // Load the image from disk.
    //
    // STBI_rgb_alpha forces the image into RGBA format
    // so OpenGL always receives 4 color channels.
    unsigned char* textureData =
        stbi_load(
            texturePath.c_str(),
            &atlasWidth,
            &atlasHeight,
            &channels,
            STBI_rgb_alpha
        );


    if (textureData == nullptr)
    {
        std::cout
            << "Failed to load texture atlas: "
            << texturePath
            << "\n";


        return false;
    }


    // --------------------------------------------------------
    // Create the OpenGL texture
    // --------------------------------------------------------

    glGenTextures(
        1,
        &atlasTexture
    );


    glBindTexture(
        GL_TEXTURE_2D,
        atlasTexture
    );


    // --------------------------------------------------------
    // Texture filtering
    // --------------------------------------------------------

    // NEAREST keeps pixel-art textures sharp instead of
    // smoothing the pixels together.
    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MIN_FILTER,
        GL_NEAREST
    );


    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MAG_FILTER,
        GL_NEAREST
    );


    // --------------------------------------------------------
    // Texture wrapping
    // --------------------------------------------------------

    // Clamp texture coordinates to the edge of the image
    // instead of repeating the texture.
    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_WRAP_S,
        GL_CLAMP_TO_EDGE
    );


    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_WRAP_T,
        GL_CLAMP_TO_EDGE
    );


    // --------------------------------------------------------
    // Upload image to GPU
    // --------------------------------------------------------

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        atlasWidth,
        atlasHeight,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        textureData
    );


    // Generate smaller versions of the texture.
    glGenerateMipmap(
        GL_TEXTURE_2D
    );


    // STB's image data is no longer needed after
    // the pixels have been copied to GPU memory.
    stbi_image_free(
        textureData
    );


    std::cout
        << "Texture atlas loaded: "
        << atlasWidth
        << " x "
        << atlasHeight
        << "\n";


    return true;
}


// ============================================================
// Load normal texture
//
// Loads a standalone image and creates an OpenGL texture.
//
// This is currently used for textures such as UI images.
//
// Returns:
// OpenGL texture ID on success
// 0 on failure
// ============================================================

unsigned int TextureManager::loadTexture(
    const std::string& texturePath
)
{
    int width;
    int height;
    int channels;


    // Load the image and convert it to RGBA.
    unsigned char* textureData =
        stbi_load(
            texturePath.c_str(),
            &width,
            &height,
            &channels,
            STBI_rgb_alpha
        );


    if (textureData == nullptr)
    {
        std::cout
            << "Failed to load texture: "
            << texturePath
            << "\n";


        return 0;
    }


    // --------------------------------------------------------
    // Create OpenGL texture
    // --------------------------------------------------------

    unsigned int textureID;


    glGenTextures(
        1,
        &textureID
    );


    glBindTexture(
        GL_TEXTURE_2D,
        textureID
    );


    // Keep pixel-art textures sharp.
    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MIN_FILTER,
        GL_NEAREST
    );


    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MAG_FILTER,
        GL_NEAREST
    );


    // Prevent the image from repeating beyond its edges.
    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_WRAP_S,
        GL_CLAMP_TO_EDGE
    );


    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_WRAP_T,
        GL_CLAMP_TO_EDGE
    );


    // Copy the image into GPU memory.
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        width,
        height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        textureData
    );


    // Free the original CPU-side image data.
    stbi_image_free(
        textureData
    );


    return textureID;
}


// ============================================================
// Atlas information
// ============================================================

// Returns the OpenGL texture ID of the block atlas.
unsigned int TextureManager::getAtlasTexture() const
{
    return atlasTexture;
}


// Returns how many block texture rows exist in the atlas.
//
// Each block type occupies one tileSize-high row.
int TextureManager::getBlockCount() const
{
    return atlasHeight / tileSize;
}