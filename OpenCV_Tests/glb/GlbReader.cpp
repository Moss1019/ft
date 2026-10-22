
#include "GlbReader.h"

#include <sstream>
#include <iostream>
#include <algorithm>
#include <glm/gtc/type_ptr.hpp>

#include "Skeleton.h"
#include "Vertex.h"

GlbReader::GlbReader(std::string fileName) : m_fileName(std::move(fileName))
{
    tg3_parse_options_init(&m_options);
    tg3_error_stack_init(&m_errorStack);
    tg3_error_code err = tg3_parse_file(&m_model, &m_errorStack, m_fileName.c_str(), m_fileName.length(), &m_options);
    if (err != TG3_OK)
    {
        std::stringstream ss;
        for (size_t i = 0; i < m_errorStack.count; ++i)
        {
            ss << (m_errorStack.entries[i].message ? m_errorStack.entries[i].message : "Unknown error");
            ss << "\n";
        }
        m_error = ss.str();
        m_inError = true;
    }
}

GlbReader::~GlbReader()
{
    tg3_error_stack_free(&m_errorStack);
    tg3_model_free(&m_model);
}

void GlbReader::meshes(std::vector<std::string> &meshNames) const
{
    for (size_t i = 0; i < m_model.meshes_count; ++i)
    {
        auto mesh = m_model.meshes[i];
        meshNames.emplace_back(mesh.name.data);
    }
}

void GlbReader::attributes(std::vector<std::string> &attributes, const std::string &meshName) const
{
    for (size_t i = 0; i < m_model.meshes_count; ++i)
    {
        const auto mesh = m_model.meshes[i];
        if (const auto mN = std::string(mesh.name.data); mN == meshName)
        {
            for (size_t j = 0; j < mesh.primitives_count; ++j)
            {
                const auto prim = mesh.primitives[j];
                for (size_t k = 0; k < prim.attributes_count; ++k)
                {
                    if (auto attributeName = std::string(prim.attributes[k].key.data);
                        std::ranges::find(attributes, attributeName) == attributes.end())
                    {
                        attributes.push_back(attributeName);
                    }
                }
            }
        }
    }
}

void GlbReader::indices(std::vector<uint32_t> &indices) const
{
    if (m_inError)
    {
        return;
    }
    auto mesh = m_model.meshes[0];
    for (int j = 0; j < mesh.primitives_count; ++j)
    {
        auto prim = mesh.primitives[j];
        tg3_accessor indAcc = m_model.accessors[prim.indices];
        tg3_buffer_view indBufferView = m_model.buffer_views[indAcc.buffer_view];
        tg3_buffer indBuffer = m_model.buffers[indBufferView.buffer];
        const unsigned char *base = &indBuffer.data.data[indBufferView.byte_offset + indAcc.byte_offset];

        indices.resize(indAcc.count);
        for (size_t k = 0; k < indAcc.count; ++k)
        {
            switch (indAcc.component_type)
            {
                case TG3_COMPONENT_TYPE_UNSIGNED_BYTE:
                    indices[k] = base[k];
                    break;
                case TG3_COMPONENT_TYPE_UNSIGNED_SHORT:
                    indices[k] = reinterpret_cast<const uint16_t *>(base)[k];
                    break;
                case TG3_COMPONENT_TYPE_UNSIGNED_INT:
                    indices[k] = reinterpret_cast<const uint32_t *>(base)[k];
                    break;
            }
        }
    }
}

void GlbReader::positions(std::vector<float> &positions) const
{
    if (m_inError)
    {
        return;
    }
    auto mesh = m_model.meshes[0];
    for (int j = 0; j < mesh.primitives_count; ++j)
    {
        tg3_accessor posAcc = findAccessor(mesh.primitives[j], "POSITION");
        if (posAcc.count == 0)
        {
            continue;
        }
        tg3_buffer_view posBufferView = m_model.buffer_views[posAcc.buffer_view];
        tg3_buffer posBuffer = m_model.buffers[posBufferView.buffer];
        positions.resize(posAcc.count * 3);
        const auto *p = reinterpret_cast<const float *>(&posBuffer.data.data[
            posBufferView.byte_offset + posAcc.byte_offset]);
        for (size_t i = 0; i < posAcc.count; ++i)
        {
            positions[i * 3] = p[i * 3];
            positions[i * 3 + 1] = p[i * 3 + 1];
            positions[i * 3 + 2] = p[i * 3 + 2];
        }
    }
}

void GlbReader::normals(std::vector<float> &normals) const
{
    if (m_inError)
    {
        return;
    }
    auto mesh = m_model.meshes[0];
    for (int j = 0; j < mesh.primitives_count; ++j)
    {
        tg3_accessor norAcc = findAccessor(mesh.primitives[j], "NORMAL");
        if (norAcc.count == 0)
        {
            continue;
        }
        tg3_buffer_view norBufferView = m_model.buffer_views[norAcc.buffer_view];
        tg3_buffer norBuffer = m_model.buffers[norBufferView.buffer];
        normals.resize(norAcc.count * 3);
        const auto *p = reinterpret_cast<const float *>(&norBuffer.data.data[
            norBufferView.byte_offset + norAcc.byte_offset]);
        for (size_t i = 0; i < norAcc.count; ++i)
        {
            normals[i * 3] = p[i * 3];
            normals[i * 3 + 1] = p[i * 3 + 1];
            normals[i * 3 + 2] = p[i * 3 + 2];
        }
    }
}

void GlbReader::joints(std::vector<uint16_t> &jointIndices) const
{
    if (m_inError)
    {
        return;
    }
    auto mesh = m_model.meshes[0];
    for (int j = 0; j < mesh.primitives_count; ++j)
    {
        tg3_accessor joiAcc = findAccessor(mesh.primitives[j], "JOINTS_0");
        if (joiAcc.count == 0)
        {
            continue;
        }
        tg3_buffer_view joiBufferView = m_model.buffer_views[joiAcc.buffer_view];
        tg3_buffer joiBuffer = m_model.buffers[joiBufferView.buffer];
        const uint8_t *base = &joiBuffer.data.data[joiBufferView.byte_offset + joiAcc.byte_offset];
        jointIndices.resize(joiAcc.count * 4);
        switch (joiAcc.component_type)
        {
            case TG3_COMPONENT_TYPE_UNSIGNED_BYTE:
            {
                const auto *p = base;
                for (size_t i = 0; i < joiAcc.count * 4; ++i)
                {
                    uint16_t v = p[i];
                    jointIndices[i] = v;
                }
                break;
            }
            case TG3_COMPONENT_TYPE_UNSIGNED_SHORT:
            {
                const auto *p = reinterpret_cast<const uint16_t *>(base);
                for (size_t i = 0; i < joiAcc.count * 4; ++i)
                {
                    jointIndices[i] = p[i];
                }
                break;
            }
            case TG3_COMPONENT_TYPE_UNSIGNED_INT:
            {
                const auto *p = reinterpret_cast<const uint32_t *>(base);
                for (size_t i = 0; i < joiAcc.count * 4; ++i)
                {
                    jointIndices[i] = static_cast<uint16_t>(p[i]);
                }
                break;
            }
        }
    }
}

void GlbReader::weights(std::vector<float> &weights) const
{
    if (m_inError)
    {
        return;
    }
    auto mesh = m_model.meshes[0];
    for (size_t j = 0; j < mesh.primitives_count; ++j)
    {
        tg3_accessor weiAcc = findAccessor(mesh.primitives[j], "WEIGHTS_0");
        if (weiAcc.count == 0)
        {
            continue;
        }
        tg3_buffer_view weiBufferView = m_model.buffer_views[weiAcc.buffer_view];
        tg3_buffer weiBuffer = m_model.buffers[weiBufferView.buffer];
        const uint8_t *base = &weiBuffer.data.data[weiBufferView.byte_offset + weiAcc.byte_offset];
        weights.resize(weiAcc.count * 4);
        switch (weiAcc.component_type)
        {
            case TG3_COMPONENT_TYPE_FLOAT:
            {
                const auto *p = reinterpret_cast<const float *>(base);
                for (size_t i = 0; i < weiAcc.count * 4; ++i)
                {
                    weights[i] = p[i];
                }
                break;
            }
            case TG3_COMPONENT_TYPE_UNSIGNED_BYTE:
            {
                const auto *p = base;
                for (size_t i = 0; i < weiAcc.count * 4; ++i)
                {
                    weights[i] = static_cast<float>(p[i]) / 255.0f;
                }
                break;
            }
            case TG3_COMPONENT_TYPE_UNSIGNED_SHORT:
            {
                const auto *p = reinterpret_cast<const uint16_t *>(base);
                for (size_t i = 0; i < weiAcc.count * 4; ++i)
                {
                    weights[i] = static_cast<float>(p[i]) / 65535.0f;
                }
                break;
            }
        }
    }
}

void GlbReader::vertexData(std::vector<Vertex> &vertices) const
{
    std::vector<float> p;
    positions(p);

    std::vector<float> n;
    normals(n);

    std::vector<float> w;
    weights(w);

    std::vector<uint16_t> j;
    joints(j);

    size_t numberVertices = p.size() / 3;

    if (p.size() / 3 != n.size() / 3)
    {
        std::cout << "Something wrong with the vectors\n";
    }

    for (size_t i = 0; i < numberVertices; ++i)
    {
        vertices.emplace_back(
            p.empty() ? nullptr : &p[i * 3],
            n.empty() ? nullptr : &n[i * 3],
            w.empty() ? nullptr : &w[i * 4],
            j.empty() ? nullptr : &j[i * 4]);
    }
}

void GlbReader::skinJoints(JointInfo &out) const
{
    if (m_model.skins_count == 0) { return; }
    tg3_skin skin = m_model.skins[0];
    if (m_inError || skin.inverse_bind_matrices == -1)
    {
        return;
    }

    out.nodeIndices.resize(skin.joints_count);
    for (size_t i = 0; i < skin.joints_count; ++i)
    {
        out.nodeIndices[i] = skin.joints[i];
    }

    tg3_accessor matAcc = m_model.accessors[skin.inverse_bind_matrices];
    tg3_buffer_view matBufferView = m_model.buffer_views[matAcc.buffer_view];
    tg3_buffer matBuffer = m_model.buffers[matBufferView.buffer];
    const uint8_t *base = &matBuffer.data.data[matBufferView.byte_offset + matAcc.byte_offset];

    out.inverseBindMatrix.resize(matAcc.count);

    auto t = matAcc.type;
    auto t1 = matAcc.component_type;
    size_t stride = matBufferView.byte_stride == 0 ? sizeof(float) * 16 : matBufferView.byte_stride;

    for (size_t i = 0; i < matAcc.count; ++i)
    {
        const auto *matrixPtr = reinterpret_cast<const float *>(base + i * stride);
        out.inverseBindMatrix[i] = glm::make_mat4(matrixPtr);
    }
}

void GlbReader::nodeHierarchy(NodeInfo &out) const
{
    out.parentOf.assign(m_model.nodes_count, -1);
    out.localTransform.resize(m_model.nodes_count);
    out.names.resize(m_model.nodes_count);

    for (size_t i = 0; i < m_model.nodes_count; ++i)
    {
        const auto &n = m_model.nodes[i];

        if (n.has_matrix)
        {
            out.localTransform[i] = glm::make_mat4(n.matrix);
        }
        else
        {
            out.localTransform[i] = glm::translate(glm::mat4(1.0),
                                                   glm::vec3(static_cast<float>(n.translation[0]),
                                                             static_cast<float>(n.translation[1]),
                                                             static_cast<float>(n.translation[2])))
                                    * glm::mat4_cast(glm::quat(static_cast<float>(n.rotation[3]),
                                                               static_cast<float>(n.rotation[0]),
                                                               static_cast<float>(n.rotation[1]),
                                                               static_cast<float>(n.rotation[2])))
                                    * glm::scale(glm::mat4(1.0),
                                                 glm::vec3(static_cast<float>(n.scale[0]),
                                                           static_cast<float>(n.scale[1]),
                                                           static_cast<float>(n.scale[2])));
        }
        for (size_t c = 0; c < n.children_count; ++c)
        {
            out.parentOf[n.children[c]] = static_cast<uint32_t>(i);
            out.names[n.children[c]] = std::string(m_model.nodes[n.children[c]].name.data);
        }
        out.names[i] = std::string(m_model.nodes[i].name.data);
    }
}

void GlbReader::skeleton(Skeleton &skeleton) const
{
    for (size_t i = 0; i < m_model.nodes_count; ++i)
    {
        const auto &n = m_model.nodes[i];

        if (n.has_matrix)
        {

        }
        else
        {
            skeleton.setTranslation(i, glm::vec3(static_cast<float>(n.translation[0]),
                                                             static_cast<float>(n.translation[1]),
                                                             static_cast<float>(n.translation[2])));
            skeleton.setRotation(i, glm::quat(static_cast<float>(n.rotation[3]),
                                                               static_cast<float>(n.rotation[0]),
                                                               static_cast<float>(n.rotation[1]),
                                                               static_cast<float>(n.rotation[2])));
            skeleton.setScale(i, glm::vec3(static_cast<float>(n.scale[0]),
                                                           static_cast<float>(n.scale[1]),
                                                           static_cast<float>(n.scale[2])));
        }
    }
}

tg3_accessor GlbReader::findAccessor(const tg3_primitive &prim, const std::string &name) const
{
    for (size_t i = 0; i < prim.attributes_count; ++i)
    {
        if (std::string n(prim.attributes[i].key.data); n == name)
        {
            return m_model.accessors[prim.attributes[i].value];
        }
    }
    return {};
}
