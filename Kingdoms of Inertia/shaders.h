#pragma once
#include <windows.h>
#include <Glad/glad.h>
#include <stdexcept>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>
#include "defines.h"
#include <Glm/glm.hpp>

struct Shader
{
	GLenum eShaderType;
	GLhandle nHandle;
};

typedef GLhandle ShaderProgram;

namespace Shaders {
	Shader CreateShader(GLenum eType, const char* pSource);
	void DestroyProgram(ShaderProgram program);
	ShaderProgram CreateProgram(const char* pShader);
	void SetUniform(ShaderProgram program, int location, bool transpose, const glm::mat4* matrix);
}
