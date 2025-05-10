#pragma once
#include <algorithm>
#include <vector>
#include <queue>
#include "SpriteManager.h"
#include "defines.h"
#include "UIElement.h"
#include "GLMappedBuffer.h"

struct ElementOffset {
	size_t vertexOffset;
	size_t indexOffset;
};

#define QUAD_SIZE sizeof(UIVertex[4]);

class UIManager
{
	std::unique_ptr<SpriteManager> spriteManager;
	std::vector<UIElement*> elements;

	// turn vbo's into custom allocaters for gpu
	GLMappedBuffer<UIVertex> sharedVbo;
	//GLbuff sharedVbo;
	GLbuff sharedEbo;
	GLbuff sharedVao;
		
	ElementOffset nextOffset;
	int maxElements;
	int index;
public:
	UIManager(int maxElements): spriteManager(_SpriteManager::LoadSprites("Resource/ui.spr", "Resource/Sprites")), sharedVbo(maxElements) {
		index = 0;
		this->maxElements = maxElements;

		elements = std::vector<UIElement*>(maxElements);
		
		glCreateBuffers(1, &sharedEbo);
		glCreateVertexArrays(1, &sharedVao);

		CheckGLExpression(glNamedBufferData(sharedEbo, maxElements * sizeof(uint32_t[6]), nullptr, GL_STATIC_DRAW));

		CheckGLExpression(glVertexArrayVertexBuffer(sharedVao, 0, sharedVbo.Handle(), 0, sizeof(UIVertex)));

		CheckGLExpression(glEnableVertexArrayAttrib(sharedVao, 0));
		CheckGLExpression(glVertexArrayAttribFormat(sharedVao, 0, 3, GL_FLOAT, GL_FALSE, 0));
		CheckGLExpression(glVertexArrayAttribBinding(sharedVao, 0, 0));

		CheckGLExpression(glEnableVertexArrayAttrib(sharedVao, 1));
		CheckGLExpression(glVertexArrayAttribFormat(sharedVao, 1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2));
		CheckGLExpression(glVertexArrayAttribBinding(sharedVao, 1, 0));

		CheckGLExpression(glVertexArrayElementBuffer(sharedVao, sharedEbo));


		for (size_t i = 0; i < maxElements; i += 6) {
			uint32_t baseVertex = i / 6 * 4;  // Each quad uses 4 vertices

			uint32_t indices[6] = {
				baseVertex + 0, baseVertex + 1, baseVertex + 3,
				baseVertex + 1, baseVertex + 2, baseVertex + 3
			};

			CheckGLExpression(glNamedBufferSubData(sharedEbo, i * sizeof(uint32_t), sizeof(indices), indices));
		}

		nextOffset = { 0, 0 };
	}
	~UIManager() {
		for (UIElement* element : elements)
			delete element;

		glDeleteBuffers(1, &sharedEbo);
        glDeleteVertexArrays(1, &sharedVao);

	}
	void Update(DWORD64 delta) {
		for (size_t i = 0; i < index; i++) 
			elements[i]->Update(delta);
	}

	void Draw() {
		// Always bind to the first slot.
		spriteManager->Bind(0);
		glBindVertexArray(sharedVao);
		CheckGLExpression(glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(index * 6), GL_UNSIGNED_INT, 0));
		
	}

	SpriteManager* GetSprites() {
		return spriteManager.get();
	}

	void SetSprite(UIElement* element, const char* spriteName) {
		const Sprite* sprite = spriteManager->GetSprite(spriteName);
		if (!sprite) return;

		float uStart = sprite->offsetX / (float)spriteManager->width;
		float vStart = sprite->offsetY / (float)spriteManager->height;
		float uEnd = (sprite->offsetX + sprite->width) / (float)spriteManager->width;
		float vEnd = (sprite->offsetY + sprite->height) / (float)spriteManager->height;

		element->SetUV(uStart, vStart, uEnd, vEnd);
	}

	UIElement* AllocateElement(float x, float y, float width, float height) {
		size_t zIndex = nextOffset.vertexOffset / sizeof(UIVertex[4]);

		UIVertex* mappedPtr = reinterpret_cast<UIVertex*>(reinterpret_cast<uint8_t*>(sharedVbo.Data()) + nextOffset.vertexOffset);

		
		UIElement* element = new UIElement(zIndex, x, y, width, height, mappedPtr);

		nextOffset.vertexOffset += sizeof(UIVertex[4]);
		nextOffset.indexOffset += 6;

		elements[index++] = element;
		
		return element;
	}

	void DeallocateElement(UIElement* element) {


		size_t elementIndex = SIZE_MAX;

		for (size_t i = 0; elements[i] != nullptr; i++) {
			if (elements[i] == element)
				elementIndex = i;
		}

		if (SIZE_MAX == elementIndex)
			throw std::runtime_error("UIelement is not part of manager.");


		size_t lastIndex = 0;

		while (elements[lastIndex++] != nullptr);

		lastIndex--; // off by one error.
		
		if (element == elements[lastIndex]) {
			delete element;
			elements[lastIndex] = nullptr;
			return;
		}

		if (lastIndex != 0) {
			UIVertex* backMapped = element->mappedPtr;
			UIVertex* frontMapped = elements[lastIndex]->mappedPtr;

			memcpy(backMapped, frontMapped, sizeof(UIVertex[4]));
			memset(frontMapped, NULL, sizeof(UIVertex[4]));

			elements[elementIndex] = elements[lastIndex];

			elements[lastIndex] = nullptr;
		}

		delete element;
	}

};

