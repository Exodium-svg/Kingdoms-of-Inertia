#pragma once
#include <stdexcept>
#include "defines.h"
#include "SpriteManager.h"

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
	Rect rect;
	
	
	void UpdateBuffers();
	public:
		float uStart;
		float vStart;
		float uEnd;
		float vEnd;
		char id[50];
		size_t vertexOffset; // in floats
		size_t indexOffset;  // in indices
		bool shouldUpdate;
		UIElement(float x, float y, float width, float height, size_t vertexOffset, size_t indexOffset, GLbuff vbo, GLbuff ebo);
		//TODO: create another constructor with UV support.
		void SetId(const char* id);
		void SetUV(float uStart, float vStart, float uEnd, float vEnd);
		void Relocate(float x, float y, float width, float height);
		virtual void Update(DWORD64 delta);
		Rect GetRect() const { return rect; }
};

