#include "UIElement.h"

void UIElement::UpdateVertices()
{
    UIVertex vertices[4] = {
        // Top right
        { { rect.x + rect.width, rect.y + rect.height }, { uvMap.uEnd, uvMap.vEnd } },

        // Bottom right
        { { rect.x + rect.width, rect.y }, { uvMap.uEnd, uvMap.vStart } },

        // Bottom left
        { { rect.x, rect.y }, { uvMap.uStart, uvMap.vStart } },

        // Top left
        { { rect.x, rect.y + rect.height }, { uvMap.uStart, uvMap.vEnd } },
    };
    
    memcpy(mappedPtr, vertices, sizeof(vertices));
}

UIElement::UIElement(size_t zIndex, float x, float y, float width, float height, UIVertex* mappedPtr):
    mappedPtr(mappedPtr), zIndex(zIndex)
{
    id[0] = '\0'; // terminate the string immediately.
    
    uvMap = {0};
    rect = { x, y, width, height };

    shouldUpdate = true;
}
void UIElement::SetId(const char* id)
{
    size_t size;
    size_t strLen = strlen(id);

    if (strLen >= sizeof(this->id) - 2)
        size = sizeof(this->id) - 2;
    else
        size = strLen;

    memcpy(this->id, id, size);
    this->id[size] = '\0';
}
void UIElement::SetUV(float uStart, float vStart, float uEnd, float vEnd)
{
    uvMap = {
        uStart,
        vStart,
        vEnd,
        uEnd,
    };
    
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
        UpdateVertices();
        shouldUpdate = false;
    }
}
