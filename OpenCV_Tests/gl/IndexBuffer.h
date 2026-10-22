#pragma once

#include <vector>

#include "glad/glad.h"

class IndexBuffer
{
public:
    IndexBuffer();

    IndexBuffer(const IndexBuffer &other) = delete;

    IndexBuffer operator=(const IndexBuffer &other) = delete;

    ~IndexBuffer();

    [[nodiscard]]
    GLuint handle() const;

    [[nodiscard]]
    unsigned number() const;

    void fill(const std::vector<GLuint> &data);
private:
    GLuint m_handle = 0;

    unsigned m_number = 0;
};
