#include "Light.h"

#include <cstring>

Light::Light(const std::vector<float> &position, const std::vector<float> &color)
{
    std::memcpy(m_position, position.data(), 3 * sizeof(float));
    std::memcpy(m_color, color.data(), 3 * sizeof(float));
}

float *Light::position()
{
    return m_position;
}

float *Light::color()
{
    return m_color;
}