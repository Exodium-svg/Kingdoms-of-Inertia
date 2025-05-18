#pragma once
#include "Renderer2D.h"
class Renderer2D;

class Text2D {
	Renderer2D* renderer;

	int x, y, z, size;
	
	size_t spriteCount;
	// I hate being cache friendly, I prefer being fucking cringe friendly.
	Sprite** sprites;

	char* characters;
	void GetSprites(const char* characters, int size);
	void ClearSprites();
public:
	Text2D(const char* characters, int x, int y, int z, Renderer2D* renderer, int size = 16);
	void SetText(const char* characters);
	void Resize(int size) { this->size = size; };
	void SetY(int y) { this->y = y; }
	void SetX(int x) { this->x = x; }
	void SetZ(int z) { this->z = z; }
	void SetLocation(int x, int y, int z) { this->x = x; this->y = y; this->z = z; }
	void Update();
	~Text2D();
};


