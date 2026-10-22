#include "Engine.h"

#include <bits/this_thread_sleep.h>
#include <glm/gtc/type_ptr.hpp>

#include "GameObject.h"
#include "../gl/Program.h"
#include "../gl/Shader.h"
#include "../gl/VertexBuffer.h"
#include "../gl/IndexBuffer.h"
#include "../gl/Light.h"
#include "../gl/Window.h"
#include "../glb/Vertex.h"

Engine::Engine(int width, int height)
{
    m_window = new Window(width, height);
    if (m_window->inError())
    {
        m_inError = true;
        m_error = m_window->error();
        return;
    }
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glViewport(0, 0, m_window->width(), m_window->height());

    m_vertexShader = new Shader(Shader::Type::Vertex, "vertex.glsl");
    if (m_vertexShader->inError())
    {
        m_inError = true;
        m_error = m_vertexShader->error();
        return;
    }
    m_fragmentShader = new Shader(Shader::Type::Fragment, "fragment.glsl");
    if (m_fragmentShader->inError())
    {
        m_inError = true;
        m_error = m_fragmentShader->error();
        return;
    }

    m_program = new Program();
    m_program->attachShader(*m_vertexShader);
    m_program->attachShader(*m_fragmentShader);
    m_program->activate();
    if (m_program->inError())
    {
        m_inError = true;
        m_error = m_program->error();
        return;
    }
}

Engine::~Engine()
{
    delete m_window;
    delete m_vertexBuffer;
    delete m_indexBuffer;
    delete m_program;
    delete m_vertexShader;
    delete m_fragmentShader;
}

bool Engine::run()
{
    if (m_inError) return false;
    m_vertexBuffer = new VertexBuffer(Vertex::s_totalBytes);
    char buffer[m_vertices.size() * Vertex::s_totalBytes];
    Vertex::serialize(buffer, m_vertices);
    m_vertexBuffer->fill(buffer, m_vertices.size() * Vertex::s_totalBytes);

    m_vertexBuffer->activate("aPosition", *m_program, 3, GL_FLOAT, Vertex::s_positionOffset);
    m_vertexBuffer->activate("aNormal", *m_program, 3, GL_FLOAT, Vertex::s_normalOffset);
    m_vertexBuffer->activate("aWeights", *m_program, 4, GL_FLOAT, Vertex::s_weightOffset);
    m_vertexBuffer->activate("aJoints", *m_program, 4, GL_UNSIGNED_SHORT, Vertex::s_jointsOffset);

    m_indexBuffer = new IndexBuffer();
    m_indexBuffer->fill(m_indices);

    Light l({0.0f, 10.0f, 10.0f}, {1.0f, 0.5f, 0.5f});
    m_program->enableVector3("uLightPos", 1, l.position());
    m_program->enableVector3("uLightColor", 1, l.color());

    const float aspectRatio = static_cast<GLfloat>(m_window->width()) / static_cast<GLfloat>(m_window->height());
    glm::mat4 p = glm::perspective<float>(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
    m_program->enableMatrix4fv("uProjection", 1, glm::value_ptr(p));

    auto v = glm::lookAt(glm::vec3(0.0f, 0.0f, 10.0f),
                         glm::vec3(0.0f, 0.0f, 0.0f),
                         glm::vec3(0.0f, 1.0f, 0.0f));
    m_program->enableMatrix4fv("uView", 1, glm::value_ptr(v));

    if (m_program->inError())
    {
        m_inError = true;
        m_error = m_program->error();
        return false;
    }

    while (m_window->isOpen())
    {
        if (m_window->run([&](double dt)
        {
            render(dt);
        }))
        {
            // Render loop ran
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    return true;
}

void Engine::render(const double dt) const
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (const auto &go : m_gameObjects)
    {
        go->update(dt);
        go->render(*m_program);
    }
}

GameObjectInfo Engine::addGameObjectModel(const std::vector<Vertex> &vertices, const std::vector<GLuint> &indices)
{
    const auto vertexOffset = m_vertices.size();
    for (const auto &i : indices)
    {
        m_indices.push_back(i + vertexOffset);
    }
    for (const auto &v : vertices)
    {
        m_vertices.push_back(v);
    }
    return GameObjectInfo(vertexOffset, indices.size());
}

void Engine::addGameObject(GameObject *gameObject)
{
    m_gameObjects.push_back(gameObject);
}
