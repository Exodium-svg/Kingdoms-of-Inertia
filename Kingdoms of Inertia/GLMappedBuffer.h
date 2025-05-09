#pragma once
#include <stdexcept>
#include "defines.h"
template<typename T>
class GLMappedBuffer
{
    GLbuff handle;
    GLenum bufferType;
    size_t size;
    T* mappedPtr;
public:
    GLMappedBuffer<T>(size_t size, GLenum usage = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT): size(size) {
        CheckGLExpression(glCreateBuffers(1, &handle));
        CheckGLExpression(glNamedBufferStorage(handle, size, nullptr, usage));

        mappedPtr = static_cast<T*>(glMapNamedBufferRange(handle, NULL, size, usage));

        if (mappedPtr == nullptr)
            throw std::runtime_error("Failed to map buffer persistently");
    }

    ~GLMappedBuffer() {
        if (mappedPtr)
            glUnmapNamedBuffer(handle);

        glDeleteBuffers(1, &handle);
    }

    T* Data() { return mappedPtr; }
    const size_t Size() { return size; }
};

