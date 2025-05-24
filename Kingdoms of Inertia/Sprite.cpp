#include "Sprite.h"

void Sprite::UpdateVertices()
{
    UIVertex vertices[4] = {
        // Top right
        { { rect.x + rect.width, rect.y + rect.height, zIndex }, { uvMap.uEnd, uvMap.vEnd } },

        // Bottom right
        { { rect.x + rect.width, rect.y, zIndex }, { uvMap.uEnd, uvMap.vStart } },

        // Bottom left
        { { rect.x, rect.y, zIndex }, { uvMap.uStart, uvMap.vStart } },

        // Top left
        { { rect.x, rect.y + rect.height, zIndex }, { uvMap.uStart, uvMap.vEnd } },
    };
    
    memcpy(mappedPtr, vertices, sizeof(vertices));
}

Sprite::Sprite(int zIndex, int x, int y, int width, int height, UIVertex* mappedPtr):
    mappedPtr(mappedPtr), zIndex(zIndex), uvMap({0}), rect(static_cast<float>(x), static_cast<float>(y), static_cast<float>(width), static_cast<float>(height)), 
    shouldUpdate(true)
{}

Sprite::Sprite(int x, int y, int zIndex, int width, int height, UIVertex* mappedPtr, UvMap uvMap):
    mappedPtr(mappedPtr), zIndex(zIndex), uvMap(uvMap), rect({ static_cast<float>(x), static_cast<float>(y), static_cast<float>(width), static_cast<float>(height) }), 
    shouldUpdate(true)
{}

void Sprite::SetUV(float uStart, float vStart, float uEnd, float vEnd)
{
    uvMap = {
        uStart,
        vStart,
        vEnd,
        uEnd,
    };

    
    
    shouldUpdate = true;
}

void Sprite::Relocate(int x, int y, int zIndex, int width, int height)
{
    rect = { static_cast<float>(x), static_cast<float>(y), static_cast<float>(width), static_cast<float>(height)};
    this->zIndex = -zIndex;
    shouldUpdate = true;
}

void Sprite::Update()
{
    if (true == shouldUpdate) {
        UpdateVertices();
        shouldUpdate = false;
    }
}
