#pragma once
#include <vector>
#include <string>
#include "defines.h"
#include "Texture2d.h"
#include <stb_rect_pack.h>
#include <stb_image.h>
#include <stb_image_resize.h>
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
		glBindTextureUnit(textureSlot, texture.handle);
	}
	const Sprite* GetSprite(char* name);
};


namespace _SpriteManager {
	SpriteManager* LoadSprites(const char* sprfile, const char* spriteDir);
}
