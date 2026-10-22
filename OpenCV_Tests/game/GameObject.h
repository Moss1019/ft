#pragma once

#include <string>

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include "GameObjectInfo.h"
#include "Transform.h"

class Skeleton;
class Program;

class GameObject
{
public:
    explicit GameObject(const GameObjectInfo &info);

    GameObject(const GameObject &) = delete;

    GameObject &operator=(const GameObject &) = delete;

    virtual ~GameObject() = default;

    [[nodiscard]]
    uint32_t id() const { return m_id; }

    [[nodiscard]]
    const std::string &name() const { return m_name; }

    void setName(std::string name) { m_name = std::move(name); }

    [[nodiscard]]
    Transform &transform() { return m_transform; }

    [[nodiscard]]
    const Transform &transform() const { return m_transform; }

    [[nodiscard]]
    const glm::vec3 &position() const { return m_transform.position(); }

    void setPosition(const glm::vec3 &p) { m_transform.setPosition(p); }

    [[nodiscard]]
    bool active() const { return m_active; }

    void setActive(bool a) { m_active = a; }

    [[nodiscard]]
    bool visible() const { return m_visible; }

    void setVisible(bool v) { m_visible = v; }

    virtual void render(Program &program);

    virtual void update(double dt);

protected:
    std::vector<glm::mat4> m_jointMatrices;

    virtual void onUpdate(double dt) = 0;

    virtual void onPreRender() {}

private:
    uint32_t m_id;

    std::string m_name;

    Transform m_transform;

    GameObjectInfo m_metaData;

    const void *m_indexOffset;

    bool m_active = true;

    bool m_visible = true;

    static uint32_t nextId();
};
