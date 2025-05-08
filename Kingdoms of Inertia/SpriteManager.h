#pragma once
#include <vector>
#include <string>
#include "defines.h"
#include "Texture2d.h"



struct Sprite {
	const std::string name;
	const int offsetX;
	const int offsetY;
	const int width;
	const int height;

	Sprite(std::string name, int offsetX, int offsetY, int width, int height): name(name), offsetX(offsetX), offsetY(offsetY), width(width), height(height){}
};

struct SpriteInfo {
	std::string name;
	int width;
	int height;

	SpriteInfo(const char* name, int width, int height): name(name), width(width), height(height){}
};

class SpriteManager
{
	Texture2d texture;
	const std::vector<Sprite> spritesInfo;

public:
	const int width;
	const int height;
	SpriteManager(Texture2d texture, std::vector<Sprite>&& spritesInfo, int width, int height);

	~SpriteManager();
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
	const Sprite* GetSprite(const char* name);
};

static void ReadAllSprites(const char* directory, std::vector<SpriteInfo>& sprites, void* rects, byte* textureAtlas, size_t size, size_t atlasWidth, size_t atlasHeight);
namespace _SpriteManager {
	SpriteManager* LoadSprites(const char* sprfile, const char* spriteDir);
}
