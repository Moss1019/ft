#include "VertexBuffer.h"

#include "Program.h"

VertexBuffer::VertexBuffer(GLsizei stride)
    : m_stride(stride)
{
    glGenBuffers(1, &m_handle);
    glBindBuffer(GL_ARRAY_BUFFER, m_handle);
}

VertexBuffer::~VertexBuffer()
{
    if (m_handle != 0)
    {
        glDeleteBuffers(1, &m_handle);
        m_handle = 0;
    }
}

GLuint VertexBuffer::handle() const
{
    return m_handle;
}

void VertexBuffer::fill(const char *data, GLsizeiptr size) const
{
    glBindBuffer(GL_ARRAY_BUFFER, m_handle);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

void VertexBuffer::activate(const std::string &name, const Program &program, GLint numComponents, GLenum type, size_t offset) const
{
    const auto attrib = glGetAttribLocation(program.handle(), name.c_str());
    glEnableVertexAttribArray(attrib);
    glBindBuffer(GL_ARRAY_BUFFER, m_handle);
    switch (type)
    {
        case GL_UNSIGNED_SHORT:
            glVertexAttribIPointer(attrib, numComponents, type, m_stride, reinterpret_cast<void *>(offset));
            break;
        default:
            glVertexAttribPointer(attrib, numComponents, type, GL_FALSE, m_stride, reinterpret_cast<void *>(offset));
    }
}
