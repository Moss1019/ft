#pragma once

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <string>
#include <functional>

class GLFWwindow;

class Window
{
public:
    explicit Window(int width, int height);

    ~Window();

    [[nodiscard]]
    int width() const { return m_width; }

    [[nodiscard]]
    int height() const { return m_height; }

    [[nodiscard]]
    bool inError() const { return m_inError; }

    [[nodiscard]]
    const std::string &error() const { return m_error; }

    [[nodiscard]]
    bool isOpen() const { return m_inError ? false : !glfwWindowShouldClose(m_window); }

    [[nodiscard]]
    GLFWwindow *window() const { return m_window; }

    bool run(const std::function<void(double)> &work);

private:
    int m_width = 0;

    int m_height = 0;

    bool m_inError = false;

    std::string m_error;

    GLFWwindow *m_window = nullptr;

    double m_lastMeasuredTime = 0.0;

    static bool sm_glfwInitialized;

    static bool sm_gladInitialized;
};
