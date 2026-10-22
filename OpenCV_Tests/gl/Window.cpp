#include "Window.h"

Window::Window(int width, int height)
    :m_width(width), m_height(height)
{
    m_lastMeasuredTime = glfwGetTime();

    if (!sm_glfwInitialized)
    {
        if (glfwInit() != GLFW_TRUE)
        {
            m_inError = true;
            m_error = "Could not initialize GLFW.";
        }
        sm_glfwInitialized = true;
    }

    if (!m_inError)
    {
        m_window = glfwCreateWindow(width, height, "glfw", nullptr, nullptr);
        glfwMakeContextCurrent(m_window);
        glfwSwapInterval(1);

        if (!sm_gladInitialized)
        {
            if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
            {
                m_inError = true;
                m_error = "Could not initialize GLAD.";
            }
            sm_gladInitialized = true;
        }
    }
}

Window::~Window()
{
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

bool Window::run(const std::function<void(double)> &work)
{
    static double frameTime = 30.0 / 1000.0;

    glfwPollEvents();
    const auto currentTime = glfwGetTime();
    if (const auto dt = currentTime - m_lastMeasuredTime; dt > frameTime)
    {
        m_lastMeasuredTime = currentTime;
        work(dt);
        glfwSwapBuffers(m_window);
        return true;
    }
    return false;
}

bool Window::sm_glfwInitialized = false;

bool Window::sm_gladInitialized = false;
