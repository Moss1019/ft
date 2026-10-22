#pragma once

#include <string>

#include "glad/glad.h"

class Program;

class VertexBuffer
{
public:
    explicit VertexBuffer(GLsizei stride);

    VertexBuffer(const VertexBuffer &other) = delete;

    VertexBuffer &operator=(const VertexBuffer &other) = delete;

    ~VertexBuffer();

    [[nodiscard]]
    GLuint handle() const;

    void fill(const char *data, GLsizeiptr size) const;

    void activate(const std::string &name, const Program &program, GLint numComponents, GLenum type, size_t offset) const;
private:
    GLuint m_handle = 0;

    GLsizei m_stride = 0;
};
