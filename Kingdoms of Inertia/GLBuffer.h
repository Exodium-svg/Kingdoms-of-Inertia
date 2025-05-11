#pragma once
#include "defines.h"
template<typename T>
class GLBuffer
{
	GLbuff handle;
	size_t count;
	GLenum usage;
public:
	GLBuffer<T>(size_t count, GLenum usage): count(count), usage(usage) {
		CheckGLExpression(glCreateBuffers(1, &handle));
		
		CheckGLExpression(glNamedBufferData(handle, count * sizeof(T), nullptr, usage));
	}
	~GLBuffer() {
		glDeleteBuffers(1, &handle);
	}
	void Upload(const T* data, size_t count, size_t elementOffset) {
		CheckGLExpression(glNamedBufferSubData(handle, elementOffset * sizeof(T), count * sizeof(T), data));
	}
	const GLbuff Handle() { return handle; }
	const size_t Count() { return count; }
	const GLenum Usage() { return usage; }
};

