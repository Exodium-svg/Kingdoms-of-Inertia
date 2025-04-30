#pragma once
#include <stdexcept>
#include "defines.h"

struct Rect {
	float x;
	float y;
	float width;
	float height;
};

class UIElement
{
	GLbuff vbo;
	GLbuff ebo;
	GLbuff vao;
	Rect rect;
	
	
	void UpdateBuffers();
	public:
		size_t vertexOffset; // in floats
		size_t indexOffset;  // in indices
		bool shouldUpdate;
		UIElement(float x, float y, float width, float height, size_t vertexOffset, size_t indexOffset, GLbuff vbo, GLbuff ebo);
		void Relocate(float x, float y, float width, float height);
		virtual void Update(DWORD64 delta);
		Rect GetRect() const { return rect; }
};

