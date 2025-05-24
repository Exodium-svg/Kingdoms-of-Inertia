#include "Text2D.h"

void Text2D::GetSprites(const char* characters, int size)
{
	size_t strLen{};
	spriteCount = 0;

	while (characters[strLen] != '\0') {
		if (characters[strLen] != '\n')
			spriteCount++;

		strLen++;
	}

	this->characters = new char[strLen];
	strcpy(this->characters, characters);

	size_t offset{};
	size_t line{};

	sprites = new Sprite * [spriteCount];

	size_t spriteIndex{};
	for (size_t i = 0; i < strLen; i++) {
		if (characters[i] == '\n') {
			line += size;
			offset = 0;
			continue;
		}

		sprites[spriteIndex] = renderer->AllocateSprite(static_cast<float>(x + offset), static_cast<float>(y + line), static_cast<float>(size), static_cast<float>(size));;
		char glyphName[25];
		ZeroMemory(glyphName, sizeof(glyphName));

		sprintf(glyphName, "glyph_%d", characters[i]);

		renderer->SetTexture(sprites[spriteIndex], glyphName);
		//renderer->SetTexture(sprites[spriteIndex], "glyph_48");

		offset += size;
		spriteIndex++;
	}
}

void Text2D::ClearSprites()
{
	for (size_t i = 0; i < spriteCount; i++)
		renderer->DeallocateSprite(sprites[i]);
	
	delete[] characters;
	delete[] sprites;
	characters = nullptr;
}

Text2D::Text2D(const char* characters, int x, int y, int z, Renderer2D* renderer, int size) : 
	x(x), y(y), z(z), size(size), renderer(renderer)
{
	GetSprites(characters, size);
}

void Text2D::SetText(const char* characters)
{
	ClearSprites();
	GetSprites(characters, size);
}

void Text2D::Update()
{
	ClearSprites();
	GetSprites(characters, size);
}

Text2D::~Text2D()
{
	ClearSprites();
}

