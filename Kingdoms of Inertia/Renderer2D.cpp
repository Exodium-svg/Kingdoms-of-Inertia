#include "Renderer2D.h"

Renderer2D::Renderer2D(int maxSprites, int width, int height):
	spriteMap(_SpriteManager::LoadSprites("Resource/ui.spr", "Resource/Sprites", "Resource/ubuntu-mono.ttf")), sharedVbo(maxSprites),
	sharedEbo(maxSprites, GL_STATIC_DRAW), sharedVao(sharedVbo.Handle(), sharedEbo.Handle(), sizeof(UIVertex)),
	program(Shaders::CreateProgram("basic"))
{
	Resize(width, height);
	index = 0;
	this->maxSprites = maxSprites;

	sprites = std::vector<Sprite*>(maxSprites);

	sharedVao.AddAttribute(GL_FLOAT, 3, false);
	sharedVao.AddAttribute(GL_FLOAT, 2, false);

	for (size_t i = 0; i < maxSprites; i += 6) {
		uint32_t baseVertex = i / 6 * 4;  // Each quad uses 4 vertices

		uint32_t indices[6] = {
			baseVertex + 0, baseVertex + 1, baseVertex + 3,
			baseVertex + 1, baseVertex + 2, baseVertex + 3
		};

		sharedEbo.Upload(&indices, 1, i / 6);
	}

	nextOffset = { 0, 0 };
}

Renderer2D::~Renderer2D()
{
	for (Sprite* sprite : sprites)
		delete sprite;
}

void Renderer2D::Update(DWORD64 delta)
{
	for (size_t i = 0; i < index; i++)
		sprites[i]->Update();
}

void Renderer2D::Draw()
{
	// Always bind to the first slot.
	spriteMap->Bind(0);
	glUseProgram(program);
	Shaders::SetUniform(program, "projection", false, &projection);
	Shaders::SetUniform(program, "atlas", 0);
	glBindVertexArray(sharedVao.Handle());
	CheckGLExpression(glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(index * 6), GL_UNSIGNED_INT, 0));

}

void Renderer2D::Resize(int width, int height)
{
	projection = glm::ortho(
		0.0f,            // left
		(float)width,    // right
		(float)height,   // bottom
		0.0f,            // top
		-1000.0f, 1000.0f// near and far planes
	);
}

void Renderer2D::SetTexture(Sprite* sprite, const char* spriteName)
{
	const SpriteLocation* spriteLocation = spriteMap->GetSprite(spriteName);
	if (!spriteLocation) return;

	sprite->SetUV(spriteLocation->uStart, spriteLocation->vStart, spriteLocation->uEnd, spriteLocation->vEnd);
}

Text2D Renderer2D::CreateText(int x, int y, int z, const char* characters, int size) 
{ return Text2D(characters, x, y, z, this, size); }

Sprite* Renderer2D::AllocateSprite(float x, float y, float width, float height)
{
	size_t zIndex = nextOffset.vertexOffset / sizeof(UIVertex[4]);

	UIVertex* mappedPtr = reinterpret_cast<UIVertex*>(reinterpret_cast<uint8_t*>(sharedVbo.Data()) + nextOffset.vertexOffset);
	
	//TODO implement recycling
	Sprite* sprite = new Sprite(zIndex, x, y, width, height, mappedPtr);

	nextOffset.vertexOffset += sizeof(UIVertex[4]);
	nextOffset.indexOffset += 6;

	sprites[index++] = sprite;

	return sprite;
}

void Renderer2D::DeallocateSprite(Sprite* sprite)
{
	size_t spriteIndex = SIZE_MAX;

	for (size_t i = 0; sprites[i] != nullptr; i++) {
		if (sprites[i] == sprite)
			spriteIndex = i;
	}

	if (SIZE_MAX == spriteIndex)
		throw std::runtime_error("Sprite is not part of renderer2D.");


	size_t lastIndex = 0;

	while (sprites[lastIndex++] != nullptr);

	lastIndex--; // off by one error.

	if (sprite == sprites[lastIndex]) {
		delete sprite;
		sprites[lastIndex] = nullptr;
		return;
	}

	if (lastIndex != 0) {
		UIVertex* backMapped = sprite->mappedPtr;
		UIVertex* frontMapped = sprites[lastIndex]->mappedPtr;

		memcpy(backMapped, frontMapped, sizeof(UIVertex[4]));
	
		sprites[spriteIndex] = sprites[lastIndex];
		sprites[lastIndex] = nullptr;

		index--;
	}

	delete sprite;
}
