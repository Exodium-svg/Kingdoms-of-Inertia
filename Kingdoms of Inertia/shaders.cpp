#include "shaders.h"

Shader Shaders::CreateShader(GLenum eType, const char* pSource)
{
    if (GL_VERTEX_SHADER != eType && GL_FRAGMENT_SHADER != eType)
        throw std::runtime_error("Invalid shader type");

    GLuint nHandle = glCreateShader(eType);

    if (NULL == nHandle) {
        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            char buffer[128];
            snprintf(buffer, sizeof(buffer), "glCreateShader failed (error: 0x%X)", error);
            MessageBoxA(nullptr, buffer, "OpenGL Error", MB_OK | MB_ICONERROR);
        }

        throw std::runtime_error("glCreateShader failed");
    }


    glShaderSource(nHandle, 1, &pSource, NULL);

    glCompileShader(nHandle);

    GLint nShaderCompiled;

    glGetShaderiv(nHandle, GL_COMPILE_STATUS, &nShaderCompiled);

    if (GL_TRUE != nShaderCompiled) {
        GLsizei nLength = 0;

        char message[1024];
        glGetShaderInfoLog(nHandle, sizeof(message), &nLength, message);

        MessageBoxA(nullptr, message, "OpenGL Error", MB_OK | MB_ICONERROR);
    }

    Shader shader;

    shader.eShaderType = eType;
    shader.nHandle = nHandle;

    return shader;
}

void Shaders::DestroyProgram(ShaderProgram program) { glDeleteProgram(program); }

std::string ReadFile(const std::filesystem::path& filepath) {

    if (!std::filesystem::exists(filepath)) {
        throw std::runtime_error("File does not exist");
    }

    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file");
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();
}

ShaderProgram Shaders::CreateProgram(const char* pShader)
{
    std::filesystem::path path = std::filesystem::path(pShader);

    path.replace_extension(".vert");

    std::string vertSource = ReadFile(path);

    path.replace_extension(".frag");

    std::string fragSource = ReadFile(path);

    Shader vertShader = Shaders::CreateShader(GL_VERTEX_SHADER, vertSource.c_str());
    Shader fragShader = Shaders::CreateShader(GL_FRAGMENT_SHADER, fragSource.c_str());

    GLint nHandle = glCreateProgram();

    glAttachShader(nHandle, vertShader.nHandle);
    glAttachShader(nHandle, fragShader.nHandle);

    glLinkProgram(nHandle);

    GLint nLinkStatus;

    GLint linkStatus;
    glGetProgramiv(nHandle, GL_LINK_STATUS, &linkStatus);
    if (linkStatus != GL_TRUE) {
        GLsizei logLength;
        glGetProgramiv(nHandle, GL_INFO_LOG_LENGTH, &logLength);

        std::vector<char> log(logLength);
        glGetProgramInfoLog(nHandle, logLength, &logLength, log.data());

        std::string errorMessage(log.begin(), log.end());
        MessageBoxA(nullptr, errorMessage.c_str(), "OpenGL Program Linking Error", MB_OK | MB_ICONERROR);
        throw std::runtime_error("Program linking failed");
    }

    glDetachShader(nHandle, vertShader.nHandle);
    glDetachShader(nHandle, fragShader.nHandle);
    glDeleteShader(vertShader.nHandle);
    glDeleteShader(fragShader.nHandle);

    return nHandle;
}

void Shaders::SetUniform(ShaderProgram program, int location, bool transpose, const glm::mat4* matrix) 
    { CheckGLExpression(glProgramUniformMatrix4fv(program, location, 1, transpose, (const GLfloat*)matrix)); }
