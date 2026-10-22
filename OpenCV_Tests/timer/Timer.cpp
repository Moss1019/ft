
#include "Timer.h"

void Timer::start()
{
    m_start = std::chrono::steady_clock::now();
}

float Timer::end() const
{
    const auto end = std::chrono::steady_clock::now();
    return std::chrono::duration<float>(end - m_start).count();
}
