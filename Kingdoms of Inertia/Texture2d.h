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
    Texture2d CreateTexture(const byte* imageBytes, GLenum internalFormat, int width, int height, GLenum format = GL_RGBA, GLenum type = GL_UNSIGNED_BYTE) {
        GLuint handle;
        CheckGLExpression(glCreateTextures(GL_TEXTURE_2D, 1, &handle));
        CheckGLExpression(glTextureStorage2D(handle, 1, internalFormat, width, height));

        CheckGLExpression(glTextureParameteri(handle, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        CheckGLExpression(glTextureParameteri(handle, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        CheckGLExpression(glTextureParameteri(handle, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        CheckGLExpression(glTextureParameteri(handle, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

        if (imageBytes != nullptr) {
            CheckGLExpression(glTextureSubImage2D(
                handle, 0, 0, 0, width, height, format, type, imageBytes));
        }

        return Texture2d{ handle, width, height };
    }

	void DeleteTexture(Texture2d* texture) {
		glDeleteTextures(1, &texture->handle);
	}
}