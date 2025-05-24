#pragma once
#include <vector>
#include <string>
#include "defines.h"
#include "Texture2d.h"



struct SpriteLocation {
	const std::string name;
	const float uStart;
	const float vStart;
	const float uEnd;
	const float vEnd;

	SpriteLocation(std::string name, float uStart, float vStart, float uEnd, float vEnd): name(name), uStart(uStart), vStart(vStart), uEnd(uEnd), vEnd(vEnd){}
};

struct SprSpriteInfo {
	std::string path;
	std::string name;
	int width;
	int height;

	SprSpriteInfo(const char* name, const char* path, int width, int height): name(name), path(path), width(width), height(height) {}
};

class SpriteMap
{
	Texture2d texture;
	const std::vector<SpriteLocation> spritesInfo;
public:
	const int width;
	const int height;

	SpriteMap(Texture2d texture, std::vector<SpriteLocation>&& spritesInfo, int width, int height);
	~SpriteMap();
	void Bind(int textureSlot);
	const SpriteLocation* GetSprite(const char* name);
};

static void ReadAllSprites(const char* directory, std::vector<SprSpriteInfo>& sprites, void* rects, byte* textureAtlas, size_t size, size_t atlasWidth, size_t atlasHeight);
namespace _SpriteManager {
	std::unique_ptr<SpriteMap> LoadSprites(const char* sprfile, const char* spriteDir, const char* fontfile);
}
