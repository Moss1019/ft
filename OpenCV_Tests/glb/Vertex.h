#pragma once

#include <vector>
#include <cstdint>
#include <ostream>

class Vertex
{
public:
    static const int s_positionOffset;

    static const int s_normalOffset;

    static const int s_weightOffset;

    static const int s_jointsOffset;

    static const int s_totalBytes;

    static void serialize(char *buffer, const std::vector<Vertex> &vertices);

    static void deserialize(std::vector<Vertex> &vertices, const int count, const char *buffer);

    explicit Vertex(const float *position, const float *normal, const float *weights, const uint16_t *joints);

    friend std::ostream &operator<<(std::ostream &os, const Vertex &v);

    void copyTo(char *buffer) const;
private:
    float m_position[3]{};

    float m_normal[3]{};

    float m_weights[4]{};

    uint16_t m_joints[4]{};
};
