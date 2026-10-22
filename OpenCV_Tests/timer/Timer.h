#pragma once
#include <chrono>

class Timer
{
public:
    void start();

    [[nodiscard]]
    float end() const;

private:
    std::chrono::time_point<std::chrono::steady_clock> m_start;
};