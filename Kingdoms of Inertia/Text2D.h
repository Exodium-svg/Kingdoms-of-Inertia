#pragma once
#include "Renderer2D.h"

struct LetterSettings {
	int width;
	int height;
	int margin;
	int padding;
};

struct Text2D {
	int x, y;
	size_t length;
	Sprite** sprites;
	const char* characters;
};


class Font {
	Renderer2D* renderer;
	LetterSettings settings;
	std::vector<Text2D> texts;
public:
	Font(Renderer2D* renderer) : renderer(renderer), texts(1024) {
		settings = {
			16,
			16,
			0,
			0,
		};
	}

	void SetSettings(LetterSettings settings) {
		this->settings = settings;
	}

	Text2D* CreateText(int x, int y, const char* text, size_t length = 0) {

		size_t strLen = 0;
		size_t spriteLen = 0;
		if (0 == length)
			strLen = strlen(text);
		else
			strLen = length;
		
		size_t i = 0;
		while (text[i] != '\0') {
			if (text[i++] != '\n')
				spriteLen++;
		}

		size_t offset = 0;
		size_t line = 0;
		Sprite** sprites = new Sprite * [spriteLen];
		// logical error as \n will still be counted in strlen so we will go out of memory.
		for (size_t i = 0; i < strLen; i++) {
			const char c = text[i];

			if ('\n' == c) {
				line++;
				offset = 0;
				continue;
			}

			sprites[i] = renderer->AllocateSprite(
				x,
				y,
				settings.width * offset, (line == 0) ? settings.height : settings.height * line
			);

			char spriteName[2]{ c , '\0' };
			renderer->SetTexture(sprites[i], spriteName);
		}

		char* characters = new char[strLen++];
		strcpy(characters, text);

		texts.push_back(Text2D{x, y, strLen, sprites, characters });
		return &texts.back();
	}

	void DestroyText(Text2D* text) {
		for (size_t i = 0; i < texts.size(); i++) {

			if (texts[i].sprites == text->sprites)
				for (size_t j = 0; j < text->length; j++)
					renderer->DeallocateSprite(text->sprites[j]);


			delete[] text->sprites;
			delete[] text->characters;
			break;
		}

	}
};