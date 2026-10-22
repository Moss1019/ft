#pragma once

#include <map>
#include <string>

#include "glad/glad.h"


class Shader;

class Program
{
public:
    Program();

    Program(const Program& program) = delete;

    Program operator=(const Program& program) = delete;

    ~Program();

    [[nodiscard]]
    GLint handle() const;

    [[nodiscard]]
    bool inError() const;

    [[nodiscard]]
    const std::string &error() const;

    void attachShader(const Shader &shader);

    void detachShader(const Shader &shader);

    void activate();

    void enableVector3(const std::string &name, const int count, GLfloat *data);

    void enableMatrix4fv(const std::string &name, const int count, GLfloat *data);

    void enableMatrix3fv(const std::string &name, const int count, GLfloat *data);
private:
    GLint m_handle;

    bool m_inError = false;

    std::string m_error;

    std::map<std::string, GLint> m_uniforms;
};