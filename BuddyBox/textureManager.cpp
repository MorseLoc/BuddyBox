#include "TextureManager.h"

#include <glad/glad.h>
#include <stb_image.h>

#include <iostream>


TextureManager::TextureManager()
{
	atlasTexture = 0;

	atlasWidth = 0;
	atlasHeight = 0;
}


bool TextureManager::loadAtlas(
	const std::string& texturePath
)
{
	{
		int channels;

		unsigned char* textureData = stbi_load(
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

		glGenTextures(
			1,
			&atlasTexture
		);

		glBindTexture(
			GL_TEXTURE_2D,
			atlasTexture
		);

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

		glGenerateMipmap(
			GL_TEXTURE_2D
		);

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
}

unsigned int TextureManager::getAtlasTexture() const
{
	return atlasTexture;
}


int TextureManager::getBlockCount() const
{
	return atlasHeight / tileSize;
}