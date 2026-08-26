#pragma once

#include <string>


class TextureManager
{
public:

	TextureManager();

	// Loads the block texture atlas.
	bool loadAtlas(
		const std::string& texturePath
	);

	// Loads a normal 2D texture and returns its OpenGL texture ID.
	unsigned int loadTexture(
		const std::string& texturePath
	);

	// Returns the OpenGL texture ID
	// for the block atlas.
	unsigned int getAtlasTexture() const;

	// Returns how many block rows
	// exist inside the atlas.
	int getBlockCount() const;


private:

	// OpenGL ID for artdex.png.
	unsigned int atlasTexture;

	// Size of the loaded atlas.
	int atlasWidth;
	int atlasHeight;

	// Every sprite is 16 x 16.
	static const int tileSize = 16;

	// Every block has six face sprites.
	static const int facesPerBlock = 6;
};