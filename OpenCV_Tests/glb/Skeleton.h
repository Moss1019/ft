#pragma once

#include <vector>
#include <glm/fwd.hpp>

struct NodeInfo;
struct JointInfo;

class Skeleton
{
public:
    explicit Skeleton(const NodeInfo &nodeInfo, const JointInfo &jointInfo);

    void setTranslation(int32_t idx, const glm::vec3 &translation);

    void setRotation(int32_t idx, const glm::quat &rotation);

    void setScale(int32_t idx, const glm::vec3 &scale);

    [[nodiscard]]
    glm::mat4 localTransform(int32_t idx) const;

    [[nodiscard]]
    glm::mat4 worldTransform(int32_t idx);

    void computeJointMatrices(std::vector<glm::mat4> &out);

private:
    std::vector<int32_t> m_parentOf{};

    std::vector<int32_t> m_jointNodeIndices{};

    std::vector<glm::mat4> m_inverseBind{};

    std::vector<glm::vec3> m_translations{};

    std::vector<glm::quat> m_rotations{};

    std::vector<glm::vec3> m_scales{};

    std::vector<glm::mat4> m_worldCache{};

    std::vector<bool> m_computed{};
};