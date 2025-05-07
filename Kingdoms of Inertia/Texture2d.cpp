#include "Texture2d.h"

Texture2d Texture2D::CreateTexture(const byte* imageBytes, GLenum internalFormat, int width, int height, GLenum format, GLenum type)
{
    GLuint handle;
    CheckGLExpression(glCreateTextures(GL_TEXTURE_2D, 1, &handle));
    CheckGLExpression(glTextureStorage2D(handle, 1, internalFormat, width, height));

    CheckGLExpression(glTextureParameteri(handle, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    CheckGLExpression(glTextureParameteri(handle, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    CheckGLExpression(glTextureParameteri(handle, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    CheckGLExpression(glTextureParameteri(handle, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

    if (imageBytes == nullptr)
        throw std::runtime_error("Unable to upload null data");

    CheckGLExpression(glTextureSubImage2D(handle, 0, 0, 0, width, height, format, type, imageBytes));


    return Texture2d{ handle, width, height };
}

void Texture2D::DeleteTexture(Texture2d* texture)
{
    glDeleteTextures(1, &texture->handle);
}
