#pragma once
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

class Transform
{
public:
    [[nodiscard]]
    bool isRotating() const { return m_hasTarget; }

    [[nodiscard]]
    const glm::vec3 &position() const { return m_position; }

    void setPosition(const glm::vec3 &p)
    {
        m_position = p;
        m_dirty = true;
    }

    void translate(const glm::vec3 &delta)
    {
        m_position += delta;
        m_dirty = true;
    }

    [[nodiscard]]
    const glm::quat &rotation() const { return m_rotation; }

    void setRotation(const glm::quat &q)
    {
        m_rotation = glm::normalize(q);
        m_dirty = true;
    }

    void rotate(const glm::vec3 &axis, float degrees)
    {
        setRotation(glm::angleAxis(glm::radians(degrees), glm::normalize(axis)) * m_rotation);
    }

    void setYaw(float degrees) { setRotation(glm::angleAxis(glm::radians(degrees), glm::vec3(0, 1, 0))); }

    void setTargetRotation(const glm::quat &target, float speed = 12.0f)
    {
        m_target = glm::normalize(target);
        m_turnSpeed = speed;
        m_hasTarget = true;
    }

    void lookInDirection(const glm::vec3 &dir)
    {
        if (glm::dot(dir, dir) < 1e-8f) return;
        setRotation(glm::quatLookAt(glm::normalize(dir), glm::vec3(0, 1, 0)));
    }

    void smoothLookInDirection(const glm::vec3 &dir, float speed = 12.0f)
    {
        if (glm::dot(dir, dir) < 1e-8f) return;
        setTargetRotation(glm::quatLookAt(glm::normalize(dir), glm::vec3(0, 1, 0)), speed);
    }

    [[nodiscard]]
    const glm::vec3 &scale() const { return m_scale; }

    void setScale(const glm::vec3 &s)
    {
        m_scale = s;
        m_dirty = true;
    }

    void setScale(float s) { setScale(glm::vec3(s)); }

    [[nodiscard]]
    glm::vec3 forward() const { return m_rotation * glm::vec3(0, 0, -1); }

    [[nodiscard]]
    glm::vec3 right() const { return m_rotation * glm::vec3(1, 0, 0); }

    [[nodiscard]]
    glm::vec3 up() const { return m_rotation * glm::vec3(0, 1, 0); }

    [[nodiscard]]
    glm::mat4 &matrix() const
    {
        if (m_dirty)
        {
            m_matrix = glm::translate(glm::mat4(1.0f), m_position)
                       * glm::mat4_cast(m_rotation)
                       * glm::scale(glm::mat4(1.0f), m_scale);
            m_dirty = false;
        }
        return m_matrix;
    }

    [[nodiscard]]
    glm::mat3 normalMatrix() const
    {
        return glm::transpose(glm::inverse(glm::mat3(matrix())));
    }


private:
    glm::vec3 m_position{0.0f};

    glm::quat m_rotation{1.0f, 0.0f, 0.0f, 0.0f};

    glm::vec3 m_scale{1.0f};

    glm::quat m_target{1.0f, 0.0f, 0.0f, 0.0f};
    
    float m_turnSpeed = 12.0f;

    bool m_hasTarget = false;

    mutable glm::mat4 m_matrix{1.0f};

    mutable bool m_dirty = true;
};
