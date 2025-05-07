#include "UIElement.h"

// Texture atlas ( array textures bad )
// 1. texture has to be repesented by a y offset and x offset
// 2. textures have to be stitched together into a big texture sheet ( or I can do it manually? ) 
// 3. We'd need to keep track of state in a SSBO so that we can still draw everything in 1 call.
// 4. Sprite manager.

void UIElement::UpdateBuffers()
{
    float x = rect.x;
    float y = rect.y;
    float width = rect.width;
    float height = rect.height;

    UIVertex vertices[4] = {
        // Top right
        { { x + width, y + height }, { uEnd, vEnd } },

        // Bottom right
        { { x + width, y }, { uEnd, vStart } },

        // Bottom left
        { { x, y }, { uStart, vStart } },

        // Top left
        { { x, y + height }, { uStart, vEnd } },
    };
    
    CheckGLExpression(glNamedBufferSubData(vbo, vertexOffset, sizeof(vertices), vertices));
}

UIElement::UIElement(float x, float y, float width, float height, size_t vertexOffset, size_t indexOffset, GLbuff vbo, GLbuff ebo): 
    vertexOffset(vertexOffset), indexOffset(indexOffset)
{
    ZeroMemory(id, sizeof(id));
    uStart = 0;
    vStart = 0;
    vEnd = 0;
    uEnd = 0;
    rect = { x, y, width, height };

    shouldUpdate = true;

    this->vbo = vbo;
    this->ebo = ebo;

    uint32_t baseVertex = static_cast<uint32_t>(vertexOffset / sizeof(UIVertex)); // 2 floats per vertex

    uint32_t indices[6] = {
        baseVertex + 0, baseVertex + 1, baseVertex + 3,
        baseVertex + 1, baseVertex + 2, baseVertex + 3
    };
    
    CheckGLExpression(glNamedBufferSubData(ebo, indexOffset * sizeof(uint32_t), sizeof(indices), indices));
}
void UIElement::SetId(const char* id)
{
    size_t size;
    size_t strLen = strlen(id);

    if (strLen > sizeof(this->id))
        size = sizeof(this->id);
    else
        size = strLen;
    
    ZeroMemory(this->id, sizeof(id));
    memcpy(this->id, id, size);
}
void UIElement::SetUV(float uStart, float vStart, float uEnd, float vEnd)
{
    this->uStart = uStart;
    this->vStart = vStart;
    this->vEnd = vEnd;
    this->uEnd = uEnd;

    shouldUpdate = true;
}
// introduce Z index?
void UIElement::Relocate(float x, float y, float width, float height)
{
    rect = { x, y, width, height };

    shouldUpdate = true;
}

void UIElement::Update(DWORD64 delta)
{
    if (true == shouldUpdate) {
        UpdateBuffers();
        shouldUpdate = false;
    }
}
