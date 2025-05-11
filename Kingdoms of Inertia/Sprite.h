#pragma once
#include <stdexcept>
#include "defines.h"
#include "SpriteMap.h"
struct UIVertex {
	float positions[3];
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

class Sprite
{
	Rect rect;
	UvMap uvMap;
	
	int zIndex;
	bool shouldUpdate;
	void UpdateVertices();
public:
	UIVertex* const mappedPtr;
		
	Sprite(int zIndex, int x, int y, int width, int height, UIVertex* mappedPtr);
	Sprite(int x, int y, int zIndex, int width, int height, UIVertex* mappedPtr, UvMap uvMap);
	
	void SetUV(float uStart, float vStart, float uEnd, float vEnd);
	void MarkForUpdate() { shouldUpdate = true; }
	void Relocate(int x, int y, int zIndex, int width, int height);
	void SetZIndex(int index) { zIndex = index; shouldUpdate = true; }
	void Update();
	Rect GetRect() const { return rect; }
};

