#include "GameObject.h"

#include <glm/gtc/type_ptr.hpp>

#include "../glb/Skeleton.h"
#include "../gl/Program.h"
#include "glad/glad.h"

GameObject::GameObject(const GameObjectInfo &info)
    : m_id(nextId()), m_name("GameObject_" + std::to_string(m_id)), m_metaData(info)
{
    m_indexOffset = reinterpret_cast<const void *>(m_metaData.indexStart * sizeof(GLuint));

    m_jointMatrices.assign(2, glm::mat4(1.0f));
}

void GameObject::render(Program &program)
{
    if (!m_visible) return;

    onPreRender();

    program.enableMatrix4fv("uBindMats", static_cast<GLsizei>(m_jointMatrices.size()),
                            glm::value_ptr(m_jointMatrices[0]));
    program.enableMatrix4fv("uModel", 1, glm::value_ptr(m_transform.matrix()));

    glm::mat3 nM = m_transform.normalMatrix();
    program.enableMatrix3fv("uNormalMat", 1, glm::value_ptr(nM));

    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_metaData.indexCount),
                   GL_UNSIGNED_INT, m_indexOffset);
}

void GameObject::update(double dt)
{
    if (!m_active) return;
    onUpdate(dt);
}

uint32_t GameObject::nextId()
{
    static uint32_t counter = 0;
    return ++counter;
}
