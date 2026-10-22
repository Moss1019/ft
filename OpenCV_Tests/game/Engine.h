#pragma once

#include <string>
#include <vector>

#include "GameObjectInfo.h"
#include "glad/glad.h"

class GameObject;
class Window;
class Shader;
class Program;
class IndexBuffer;
class VertexBuffer;
class Vertex;

class Engine
{
public:
    explicit Engine(int width, int height);

    ~Engine();

    [[nodiscard]]
    bool inError() const { return m_inError; }

    [[nodiscard]]
    const std::string &error() const { return m_error; }

    bool run();

    void render(double dt) const;

    GameObjectInfo addGameObjectModel(const std::vector<Vertex> &vertices, const std::vector<GLuint> &indices);

    void addGameObject(GameObject *gameObject);

private:
    bool m_inError = false;

    std::string m_error;

    Window *m_window = nullptr;

    VertexBuffer *m_vertexBuffer = nullptr;

    IndexBuffer *m_indexBuffer = nullptr;

    std::vector<Vertex> m_vertices;

    std::vector<GLuint> m_indices;

    Program *m_program = nullptr;

    Shader *m_vertexShader = nullptr;

    Shader *m_fragmentShader = nullptr;

    std::vector<GameObject *> m_gameObjects;
};
