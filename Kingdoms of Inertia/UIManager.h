#pragma once
#include <vector>
#include <queue>
#include "defines.h"
#include "UIElement.h"


struct ElementOffset {
	size_t vertexOffset;
	size_t indexOffset;
};

class UIManager
{
	GLbuff sharedVbo;
	GLbuff sharedEbo;
	GLbuff sharedVao;
	std::vector<UIElement*> elements;
	
	ElementOffset nextOffset;
	int maxElements;
	int index;
public:
	UIManager(int maxElements) {
		index = 0;
		this->maxElements = maxElements;

		elements = std::vector<UIElement*>(maxElements);
		GLbuff buffers[2];
		
		glCreateBuffers(2, buffers);
		glCreateVertexArrays(1, &sharedVao);

		sharedVbo = buffers[0];
		sharedEbo = buffers[1];

		CheckGLExpression(glNamedBufferData(sharedVbo, maxElements * 8 * sizeof(float), nullptr, GL_DYNAMIC_DRAW));
		CheckGLExpression(glNamedBufferData(sharedEbo, maxElements * 6 * sizeof(uint32_t), nullptr, GL_STATIC_DRAW));

		CheckGLExpression(glEnableVertexArrayAttrib(sharedVao, 0));
		CheckGLExpression(glVertexArrayAttribFormat(sharedVao, 0, 2, GL_FLOAT, GL_FALSE, 0));
		CheckGLExpression(glVertexArrayAttribBinding(sharedVao, 0, 0));
		CheckGLExpression(glVertexArrayVertexBuffer(sharedVao, 0, sharedVbo, 0, sizeof(float) * 2));
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
	}
	void Update(DWORD64 delta) {

		for (size_t i = 0; i < index; i++) 
			elements[i]->Update(delta);
	}

	void Draw() {
		glBindVertexArray(sharedVao);
		CheckGLExpression(glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(index * 6), GL_UNSIGNED_INT, 0));
		glBindVertexArray(0);
	}

	UIElement* AllocateElement(float x, float y, float width, float height) {
		UIElement* element = new UIElement(x, y, width, height, nextOffset.vertexOffset, nextOffset.indexOffset, sharedVbo, sharedEbo);

		nextOffset.vertexOffset += 8;
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

		float f[8]{ 0 };
		uint32_t ibo[6]{ 0 };

		CheckGLExpression(glNamedBufferSubData(sharedVbo, backOffset.vertexOffset, sizeof(f), f));
		CheckGLExpression(glNamedBufferSubData(sharedEbo, backOffset.indexOffset, sizeof(ibo), ibo));

		delete element;
	}

};

