#pragma once

#include <string>

#include "glad/glad.h"

class Shader
{
public:
    enum Type
    {
        Vertex,
        Fragment
    };

    Shader(const Shader &other) = delete;

    Shader operator=(const Shader &other) = delete;

    Shader(Type type, const std::string &fileName);

    ~Shader();

    [[nodiscard]]
    GLint handle() const;

    [[nodiscard]]
    bool inError() const;

    [[nodiscard]]
    const std::string &error() const;

private:
    GLint m_handle = 0;

    bool m_inError = false;

    std::string m_error;
};