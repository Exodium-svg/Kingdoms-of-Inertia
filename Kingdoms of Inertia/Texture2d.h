#pragma once
#include "defines.h"
#include <stdexcept>
struct Texture2d
{
	GLbuff handle;
	int width;
	int height;
};

namespace Texture2D {
    /// Creates a 2D texture using DSA.
    /// @param imageBytes Pointer to pixel data.
    /// @param internalFormat Internal texture format (e.g., GL_RGBA8).
    /// @param width Width of the texture.
    /// @param height Height of the texture.
    /// @param format Format of the pixel data (e.g., GL_RGBA).
    /// @param type Data type of the pixel data (e.g., GL_UNSIGNED_BYTE).
    Texture2d CreateTexture(const byte* imageBytes, GLenum internalFormat, int width, int height, GLenum format = GL_RGBA, GLenum type = GL_UNSIGNED_BYTE);
    void DeleteTexture(Texture2d* texture);
}