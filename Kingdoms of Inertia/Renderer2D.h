#pragma once
#include <algorithm>
#include <vector>
#include <queue>
#include <Glm/glm.hpp>
#include <Glm/gtc/matrix_transform.hpp>
#include "GLMappedBuffer.h"
#include "GLVertexArray.h"
#include "GLBuffer.h" 
#include "shaders.h"
#include "SpriteMap.h"
#include "Sprite.h"
#include "Text2D.h"
class Text2D;

struct SpriteOffset {
	size_t vertexOffset;
	size_t indexOffset;
};

class Renderer2D
{
	glm::mat4 projection;
	std::unique_ptr<SpriteMap> spriteMap;
	std::vector<Sprite*> sprites;

	ShaderProgram program;
	
	GLMappedBuffer<UIVertex> sharedVbo;
	GLBuffer<uint32_t[6]> sharedEbo;
	GLVertexArray sharedVao;
	SpriteOffset nextOffset;

	int maxSprites;
	int index;
public:
	Renderer2D(int maxSprites, int width, int height);
	~Renderer2D();
	void Update(DWORD64 delta);
	void Draw();
	void Resize(int width, int height);
	void SetTexture(Sprite* sprite, const char* spriteName);
	Text2D CreateText(int x, int y, int z, const char* characters, int size = 16);
	Sprite* AllocateSprite(float x, float y, float width, float height);
	void DeallocateSprite(Sprite* sprite);
};

