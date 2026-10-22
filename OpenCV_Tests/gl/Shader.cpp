//
// Created by moss on 10-10-2026.
//

#include "Shader.h"

#include "../file/FileHandler.h"

Shader::Shader(Type type, const std::string &fileName)
{
    const auto glType = type == Type::Vertex ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER;
    m_handle = glCreateShader(glType);
    FileHandler fh(fileName);
    const auto source = fh.readFile();
    const auto sourceChars = source.c_str();
    glShaderSource(m_handle, 1, &sourceChars, nullptr);
    glCompileShader(m_handle);
    auto status = GL_FALSE;
    glGetShaderiv(m_handle, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE)
    {
        constexpr int bufferSize = 1024;
        char buffer[bufferSize];
        auto length = 0;
        glGetShaderInfoLog(m_handle, bufferSize, &length, buffer);
        m_inError = true;
        m_error = std::string(buffer, length);
    }
}

Shader::~Shader()
{
    if (m_handle != 0)
    {
        glDeleteShader(m_handle);
        m_handle = 0;
    }
}

GLint Shader::handle() const
{
    return m_handle;
}

bool Shader::inError() const
{
    return m_inError;
}

const std::string & Shader::error() const
{
    return m_error;
}
