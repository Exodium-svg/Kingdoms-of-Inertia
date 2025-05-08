#pragma once
#include <vector>
#include <queue>
#include "SpriteManager.h"
#include "defines.h"
#include "UIElement.h"
#include <cstddef>
struct ElementOffset {
	size_t vertexOffset;
	size_t indexOffset;
};

class UIManager
{
	SpriteManager* spriteManager;
	GLbuff sharedVbo;
	GLbuff sharedEbo;
	GLbuff sharedVao;
	std::vector<UIElement*> elements;
	
	ElementOffset nextOffset;
	int maxElements;
	int index;
public:
	UIManager(int maxElements) {
		spriteManager = _SpriteManager::LoadSprites("Resource/ui.spr", "Resource/Sprites");
		index = 0;
		this->maxElements = maxElements;

		elements = std::vector<UIElement*>(maxElements);
		GLbuff buffers[2];
		
		glCreateBuffers(2, buffers);
		glCreateVertexArrays(1, &sharedVao);

		sharedVbo = buffers[0];
		sharedEbo = buffers[1];
		
		CheckGLExpression(glNamedBufferData(sharedVbo, maxElements * sizeof(UIVertex[4]), nullptr, GL_DYNAMIC_DRAW));
		CheckGLExpression(glNamedBufferData(sharedEbo, maxElements * sizeof(uint32_t[6]), nullptr, GL_STATIC_DRAW));

		CheckGLExpression(glVertexArrayVertexBuffer(sharedVao, 0, sharedVbo, 0, sizeof(UIVertex)));

		CheckGLExpression(glEnableVertexArrayAttrib(sharedVao, 0));
		CheckGLExpression(glVertexArrayAttribFormat(sharedVao, 0, 4, GL_FLOAT, GL_FALSE, 0));
		CheckGLExpression(glVertexArrayAttribBinding(sharedVao, 0, 0));

		//CheckGLExpression(glEnableVertexArrayAttrib(sharedVao, 1));
		//CheckGLExpression(glVertexArrayAttribFormat(sharedVao, 1, 2, GL_FLOAT, GL_FALSE, sizeof(UIVertex)));
		//CheckGLExpression(glVertexArrayAttribBinding(sharedVao, 1, 1));

		CheckGLExpression(glVertexArrayElementBuffer(sharedVao, sharedEbo));

		nextOffset = { 0, 0 };
	}
	~UIManager() {
		for (UIElement* element : elements)
			delete element;

		GLbuff buffs[2];
		buffs[0] = sharedVbo;
		buffs[1] = sharedEbo;

		glDeleteBuffers(sizeof(buffs), buffs);
        glDeleteVertexArrays(1, &sharedVao);

		delete spriteManager;
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
		//glBindVertexArray(0);
	}

	SpriteManager* GetSprites() {
		return spriteManager;
	}

	void SetSprite(UIElement* element, const char* spriteName) {
		const Sprite* sprite = spriteManager->GetSprite(spriteName);
		if (!sprite) return;

		float uStart = sprite->offsetX / (float)spriteManager->width;
		float vStart = sprite->offsetY / (float)spriteManager->height;
		float uEnd = (sprite->offsetX + sprite->width) / (float)spriteManager->width;
		float vEnd = (sprite->offsetY + sprite->height) / (float)spriteManager->height;

		element->SetUV(uStart, vStart, uEnd, vEnd);
		//element->SetUV(0.0f, 0.0f, 1.0f, 1.0f);
	}

	UIElement* AllocateElement(float x, float y, float width, float height) {
		UIElement* element = new UIElement(x, y, width, height, nextOffset.vertexOffset, nextOffset.indexOffset, sharedVbo, sharedEbo);

		nextOffset.vertexOffset += sizeof(UIVertex[4]);
		nextOffset.indexOffset += 6;

		elements[index++] = element;
		
		return element;
	}

	void DeallocateElement(UIElement* element) {
		bool isFound = false;
		size_t index;
		size_t end;
		for (end = 0; nullptr != elements[end]; end++) {
			if (element == elements[end]) {
				index = end;
				isFound = true;
			}
		}

		if (!isFound)
			throw std::runtime_error("UIElement is not part of UI manager????");

		// the end is the last one the index is our original one
		UIElement* backElement = elements[++end];
		elements[index] = backElement;

		ElementOffset offset{ element->vertexOffset, element->indexOffset };
		ElementOffset backOffset{ backElement->vertexOffset, backElement->indexOffset };

		backElement->vertexOffset = offset.vertexOffset;
		backElement->indexOffset = offset.indexOffset;
		backElement->shouldUpdate = true;

		float f[16]{ 0 };
		uint32_t ibo[6]{ 0 };

		CheckGLExpression(glNamedBufferSubData(sharedVbo, backOffset.vertexOffset, sizeof(f), f));
		CheckGLExpression(glNamedBufferSubData(sharedEbo, backOffset.indexOffset, sizeof(ibo), ibo));

		delete element;
	}

};

