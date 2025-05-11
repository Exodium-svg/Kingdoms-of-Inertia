#pragma once
#include "defines.h"
class GLVertexArray
{
	GLbuff handle;
	int attributes;
	size_t currentOffset;
public:
	GLVertexArray(GLbuff vbo, GLbuff ebo, size_t elementSize): attributes(0) {
		CheckGLExpression(glCreateVertexArrays(1, &handle));
		CheckGLExpression(glVertexArrayElementBuffer(handle, ebo));
		CheckGLExpression(glVertexArrayVertexBuffer(handle, 0, vbo, 0, elementSize));
	}

	~GLVertexArray() {
		glDeleteVertexArrays(1, &handle);
	}

	void AddAttribute(GLenum type, size_t size, bool normalized) {
		int index = attributes++;
		CheckGLExpression(glEnableVertexArrayAttrib(handle, index));
		CheckGLExpression(glVertexArrayAttribFormat(handle, index, size, type, normalized, currentOffset));
		CheckGLExpression(glVertexArrayAttribBinding(handle, index, 0)); // assuming buffer bound to binding 0

		currentOffset += AttributeSize(type) * size;
	}
	
	GLbuff Handle() { return handle; }

	GLVertexArray(const GLVertexArray&) = delete;
	GLVertexArray& operator=(const GLVertexArray&) = delete;

private:
	static size_t AttributeSize(GLenum type) {
		switch (type) {
		case GL_FLOAT: return sizeof(float);
		case GL_UNSIGNED_BYTE: return sizeof(uint8_t);
		case GL_BYTE: return sizeof(int8_t);
		case GL_UNSIGNED_SHORT: return sizeof(uint16_t);
		case GL_SHORT: return sizeof(int16_t);
		case GL_INT: return sizeof(int32_t);
		case GL_UNSIGNED_INT: return sizeof(uint32_t);
		case GL_DOUBLE: return sizeof(double);
		default: throw std::runtime_error("Unsupported type for VAO");
		}
	}
};

