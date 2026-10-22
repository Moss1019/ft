#pragma once

#include <vector>

class Light
{
public:
    explicit Light(const std::vector<float> &position, const std::vector<float> &color);

    [[nodiscard]]
    float *position();

    [[nodiscard]]
    float *color();
private:
    float m_position[3]{};

    float m_color[3]{};
};
