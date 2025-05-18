#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Glad/glad.h>
#include <stdint.h>
#include <memory>
#include <stdexcept>
#pragma warning(disable : 4996)

struct SmartHandle {
private:
    HANDLE hHandle;
public:
    SmartHandle(HANDLE hHandle): hHandle(hHandle) {
        if (INVALID_HANDLE_VALUE == hHandle)
            throw std::runtime_error("Invalid handle");
    }
    ~SmartHandle() { if(INVALID_HANDLE_VALUE != hHandle) CloseHandle(hHandle); }

    operator HANDLE() const { return hHandle; }

    SmartHandle& operator=(const SmartHandle& handle) = delete;
    SmartHandle& operator=(const SmartHandle&& handle) = delete;
    SmartHandle(SmartHandle&& handle) noexcept : hHandle(handle.hHandle) {
        handle.hHandle = INVALID_HANDLE_VALUE;
    }
};

inline const char* const GetGlError(GLenum error) {
    //DebugBreak();
    switch (error) {
    case GL_INVALID_ENUM:                   return "GL_INVALID_ENUM";
    case GL_INVALID_VALUE:                  return "GL_INVALID_VALUE";
    case GL_INVALID_OPERATION:              return "GL_INVALID_OPERATION";
    case GL_STACK_OVERFLOW:                 return "GL_STACK_OVERFLOW";
    case GL_STACK_UNDERFLOW:                return "GL_STACK_UNDERFLOW";
    case GL_OUT_OF_MEMORY:                  return "GL_OUT_OF_MEMORY";
    case GL_INVALID_FRAMEBUFFER_OPERATION:  return "GL_INVALID_FRAMEBUFFER_OPERATION";
    default:                                return "Unknown OpenGL Error";
    }
}



#ifdef _DEBUG
#define ENABLE_VALIDATION
#endif

#ifdef _DEBUG
#define CheckGLExpression(expression) { expression; GLenum error = glGetError(); if(GL_NO_ERROR != error) { MessageBoxA(NULL, GetGlError(error), "ERROR", MB_ICONERROR | MB_OK); throw std::runtime_error(GetGlError(error));} }
#else
#define CheckGLExpression(expression)
#endif

#ifdef _DEBUG
#define DebugPrint(text) OutputDebugStringA(text)
#else
#define DebugPrint(text)
#endif




typedef DWORD64 DeltaTimeMS;
typedef GLint GLhandle;
typedef GLuint GLbuff;
typedef unsigned char byte;