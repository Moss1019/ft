#include "Skeleton.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "GlbReader.h"

Skeleton::Skeleton(const NodeInfo &nodeInfo, const JointInfo &jointInfo)
{
    m_parentOf = nodeInfo.parentOf;
    m_jointNodeIndices = jointInfo.nodeIndices;
    m_inverseBind = jointInfo.inverseBindMatrix;
    m_worldCache.resize(nodeInfo.names.size());
    m_computed.resize(nodeInfo.names.size());
    m_translations.resize(nodeInfo.names.size());
    m_rotations.resize(nodeInfo.names.size());
    m_scales.resize(nodeInfo.names.size());
    for (size_t i = 0; i < nodeInfo.names.size(); ++i)
    {
        m_translations[i] = glm::vec3(0.0f);
        m_rotations[i] = glm::quat(glm::vec3(0.0f));
        m_scales[i] = glm::vec3(0.0f);
    }
}

void Skeleton::setTranslation(int32_t idx, const glm::vec3 &translation)
{
    m_translations[idx] = translation;
}

void Skeleton::setRotation(int32_t idx, const glm::quat &rotation)
{
    m_rotations[idx] = rotation;
}

void Skeleton::setScale(int32_t idx, const glm::vec3 &scale)
{
    m_scales[idx] = scale;
}

glm::mat4 Skeleton::localTransform(int32_t idx) const
{
    return glm::translate(glm::mat4(1.0f), m_translations[idx])
           * glm::mat4_cast(m_rotations[idx])
           * glm::scale(glm::mat4(1.0f), m_scales[idx]);
}

glm::mat4 Skeleton::worldTransform(int32_t idx)
{
    if (m_computed[idx])
    {
        return m_worldCache[idx];
    }
    glm::mat4 local = localTransform(idx);
    glm::mat4 world = (m_parentOf[idx] == -1) ? local : worldTransform(m_parentOf[idx]) * local;
    m_worldCache[idx] = world;
    m_computed[idx] = true;
    return world;
}

void Skeleton::computeJointMatrices(std::vector<glm::mat4> &out)
{
    for (auto &&i: m_computed)
    {
        i = false;
    }
    out.resize(m_jointNodeIndices.size());
    for (size_t i = 0; i < m_jointNodeIndices.size(); ++i)
    {
        out[i] = worldTransform(m_jointNodeIndices[i]) * m_inverseBind[i];
    }
}
