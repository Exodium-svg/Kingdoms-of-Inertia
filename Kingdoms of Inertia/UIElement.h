#pragma once
#include <stdexcept>
#include "defines.h"
#include "SpriteManager.h"
struct UIVertex {
	float positions[2];
	float uv[2];
};

struct UvMap {
	float uStart;
	float vStart;
	float uEnd;
	float vEnd;
};

struct Rect {
	float x;
	float y;
	float width;
	float height;
};

class UIElement
{
	Rect rect;
	UvMap uvMap;
	
	bool shouldUpdate;
	void UpdateVertices();
public:
	UIVertex* mappedPtr;
	size_t zIndex;
	char id[50];
		
	UIElement(size_t zIndex, float x, float y, float width, float height, UIVertex* mappedPtr);
	//TODO: create another constructor with UV support.
	void SetId(const char* id);
	void SetUV(float uStart, float vStart, float uEnd, float vEnd);
	void MarkForUpdate() { shouldUpdate = true; }
	void Relocate(float x, float y, float width, float height);
	virtual void Update(DWORD64 delta);
	Rect GetRect() const { return rect; }
};

