#include "Program.h"

#include "Shader.h"

Program::Program()
{
    m_handle = glCreateProgram();
}

Program::~Program()
{
    if (m_handle != 0)
    {
        glDeleteProgram(m_handle);
        m_handle = 0;
    }
}

GLint Program::handle() const
{
    return m_handle;
}

bool Program::inError() const
{
    return m_inError;
}

const std::string & Program::error() const
{
    return m_error;
}

void Program::attachShader(const Shader &shader)
{
    if (m_handle == 0)
    {
        return;
    }
    glAttachShader(m_handle, shader.handle());
}

void Program::detachShader(const Shader &shader)
{
    if (m_handle == 0)
    {
        return;
    }
    glDetachShader(m_handle, shader.handle());
}

void Program::activate()
{
    glLinkProgram(m_handle);
    glUseProgram(m_handle);
    GLint status;
    glGetProgramiv(m_handle, GL_LINK_STATUS, &status);
    if (status != GL_TRUE)
    {
        constexpr int bufferSize = 1024;
        char buffer[bufferSize];
        auto length = 0;
        glGetProgramInfoLog(m_handle, bufferSize, &length, buffer);
        m_inError = true;
        m_error = std::string(buffer, length);
    }
}

void Program::enableVector3(const std::string &name, const int count, GLfloat *data)
{
    if (!m_uniforms.contains(name))
    {
        m_uniforms[name] = glGetUniformLocation(m_handle, name.c_str());
    }
    glUniform3fv(m_uniforms[name], count, data);
}

void Program::enableMatrix4fv(const std::string &name, const int count, GLfloat *data)
{
    if (!m_uniforms.contains(name))
    {
        m_uniforms[name] = glGetUniformLocation(m_handle, name.c_str());
    }
    glUniformMatrix4fv(m_uniforms[name], count, GL_FALSE, data);
}

void Program::enableMatrix3fv(const std::string &name, const int count, GLfloat *data)
{
    if (!m_uniforms.contains(name))
    {
        m_uniforms[name] = glGetUniformLocation(m_handle, name.c_str());
    }
    glUniformMatrix3fv(m_uniforms[name], count, GL_FALSE, data);
}
