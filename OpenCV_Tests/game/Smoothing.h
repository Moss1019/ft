#pragma once

#include <glm/gtc/quaternion.hpp>

static glm::quat smoothTowards(const glm::quat &current, const glm::quat &target,
                               float speed, float dt)
{
    const float t = 1.0f - std::exp(-speed * dt);
    return glm::normalize(glm::slerp(current, target, t));
}
