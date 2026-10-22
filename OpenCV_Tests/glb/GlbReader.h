#pragma once

#include <glm/mat4x4.hpp>
#include <string>
#include <vector>

#include "tinygltf/tiny_gltf_v3.h"

class Skeleton;
class Vertex;

struct JointInfo
{
    std::vector<int> nodeIndices;

    std::vector<glm::mat4> inverseBindMatrix;
};

struct NodeInfo
{
    std::vector<int32_t> parentOf;

    std::vector<glm::mat4> localTransform;

    std::vector<std::string> names;
};

class GlbReader
{
public:
    explicit GlbReader(std::string fileName);

    ~GlbReader();

    [[nodiscard]]
    bool inError() const { return m_inError; }

    [[nodiscard]]
    const std::string &error() const { return m_error; }

    void meshes(std::vector<std::string> &meshNames) const;

    void attributes(std::vector<std::string> &attributes, const std::string &meshName) const;

    void indices(std::vector<uint32_t> &indices) const;

    void positions(std::vector<float> &positions) const;

    void normals(std::vector<float> &normals) const;

    void joints(std::vector<uint16_t> &jointIndices) const;

    void weights(std::vector<float> &weights) const;

    void vertexData(std::vector<Vertex> &vertices) const;

    void skinJoints(JointInfo &out) const;

    void nodeHierarchy(NodeInfo &out) const;

    void skeleton(Skeleton &skeleton) const;

private:
    std::string m_fileName;

    tg3_parse_options m_options{};

    tg3_error_stack m_errorStack{};

    tg3_model m_model{};

    bool m_inError = false;

    std::string m_error;

    [[nodiscard]]
    tg3_accessor findAccessor(const tg3_primitive &prim, const std::string & name) const;
};
