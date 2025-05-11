#pragma once
#include <vector>
#include <string>
#include "defines.h"
#include "Texture2d.h"



struct SpriteLocation {
	const std::string name;
	const int offsetX;
	const int offsetY;
	const int width;
	const int height;

	SpriteLocation(std::string name, int offsetX, int offsetY, int width, int height): name(name), offsetX(offsetX), offsetY(offsetY), width(width), height(height){}
};

struct SprSpriteInfo {
	std::string path;
	std::string name;
	int width;
	int height;

	SprSpriteInfo(const char* name, const char* path, int width, int height): name(name), path(path), width(width), height(height) {}
};

class Sprites
{
	Texture2d texture;
	const std::vector<SpriteLocation> spritesInfo;
public:
	const int width;
	const int height;

	Sprites(Texture2d texture, std::vector<SpriteLocation>&& spritesInfo, int width, int height);
	~Sprites();
	inline void Bind(int textureSlot) {
		int slot;

		switch (textureSlot) {
		case 0:
			slot = GL_TEXTURE0;
			break;
		case 1:
			slot = GL_TEXTURE1;
			break;
		case 2:
			slot = GL_TEXTURE2;
			break;
		default:
			throw std::runtime_error("unknown slot");
		}

		glActiveTexture(slot);
		glBindTextureUnit(textureSlot, texture.handle);
	}
	const SpriteLocation* GetSprite(const char* name);
};

static void ReadAllSprites(const char* directory, std::vector<SprSpriteInfo>& sprites, void* rects, byte* textureAtlas, size_t size, size_t atlasWidth, size_t atlasHeight);
namespace _SpriteManager {
	std::unique_ptr<Sprites> LoadSprites(const char* sprfile, const char* spriteDir);
}
