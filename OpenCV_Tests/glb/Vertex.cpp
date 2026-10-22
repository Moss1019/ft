#include "Vertex.h"

#include <cstring>

const int Vertex::s_positionOffset = 0 * sizeof(float);
const int Vertex::s_normalOffset = 3 * sizeof(float);
const int Vertex::s_weightOffset = 6 * sizeof(float);
const int Vertex::s_jointsOffset = 10 * sizeof(float);
const int Vertex::s_totalBytes = 10 * sizeof(float) + 4 * sizeof(uint16_t);

void Vertex::serialize(char *buffer, const std::vector<Vertex> &vertices)
{
    for (size_t i = 0; i < vertices.size(); ++i)
    {
        const int offset = static_cast<int>(i) * s_totalBytes;
        vertices[i].copyTo(buffer + offset);
    }
}

void Vertex::deserialize(std::vector<Vertex> &vertices, const int count, const char *buffer)
{
    vertices.clear();
    for (size_t i = 0; i < count; ++i)
    {
        const auto offset = static_cast<int>(i) * s_totalBytes;
        const auto *position = reinterpret_cast<const float *>(buffer + offset + s_positionOffset);
        const auto *normal = reinterpret_cast<const float *>(buffer + offset + s_normalOffset);
        const auto *weights = reinterpret_cast<const float *>(buffer + offset + s_weightOffset);
        const auto *joints = reinterpret_cast<const uint16_t *>(buffer + offset + s_jointsOffset);
        vertices.emplace_back(position, normal, weights, joints);
    }
}

Vertex::Vertex(const float *position, const float *normal, const float *weights, const uint16_t *joints)
{
    if (position != nullptr)
    {
        m_position[0] = position[0];
        m_position[1] = position[1];
        m_position[2] = position[2];
    }

    if (normal != nullptr)
    {
        m_normal[0] = normal[0];
        m_normal[1] = normal[1];
        m_normal[2] = normal[2];
    }

    if (weights != nullptr)
    {
        m_weights[0] = weights[0];
        m_weights[1] = weights[1];
        m_weights[2] = weights[2];
        m_weights[3] = weights[3];
    }

    if (joints != nullptr)
    {
        m_joints[0] = joints[0];
        m_joints[1] = joints[1];
        m_joints[2] = joints[2];
        m_joints[3] = joints[3];
    }
}

void Vertex::copyTo(char *buffer) const
{
    std::memcpy(buffer + s_positionOffset, m_position, 3 * sizeof(float));
    std::memcpy(buffer + s_normalOffset, m_normal, 3 * sizeof(float));
    std::memcpy(buffer + s_weightOffset, m_weights, 4 * sizeof(float));
    std::memcpy(buffer + s_jointsOffset, m_joints, 4 * sizeof(uint16_t));
}

std::ostream &operator<<(std::ostream &os, const Vertex &v)
{
    os << "Position: [" <<
            v.m_position[0] << ", " <<
            v.m_position[1] << ", " <<
            v.m_position[2] <<
            "]" <<
            " Normal: [" <<
            v.m_normal[0] << ", " <<
            v.m_normal[1] << ", " <<
            v.m_normal[2] <<
            "]" << " Weights: [" <<
            v.m_weights[0] << ", " <<
            v.m_weights[1] << ", " <<
            v.m_weights[2] << ", " <<
            v.m_weights[3] <<
            "]" <<
            " Joints: [" <<
            v.m_joints[0] << ", " <<
            v.m_joints[1] << ", " <<
            v.m_joints[2] << ", " <<
            v.m_joints[3] <<
            "]";
    return os;
}
