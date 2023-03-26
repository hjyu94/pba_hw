#include "object.h"

ObjectUPtr Object::Create()
{
    auto object = ObjectUPtr(new Object());
    if (!object->Init())
        return nullptr;
    return std::move(object);
}

bool Object::Init()
{
    float vertices[] = {
        0.5f, 0.5f, 0.0f, // top right
        0.5f, -0.5f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f, // bottom left
        -0.5f, 0.5f, 0.0f, // top left
    };
    uint32_t indices[] = { // note that we start from 0!
        0, 1, 3, // first triangle
        1, 2, 3, // second triangle
    };

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    ShaderPtr vertShader = Shader::CreateFromFile(std::string(CURRENT_SOURCE_DIR) + std::string("/shader/texture.vs"), GL_VERTEX_SHADER);
    ShaderPtr fragShader = Shader::CreateFromFile(std::string(CURRENT_SOURCE_DIR) + std::string("/shader/texture.fs"), GL_FRAGMENT_SHADER);
    if (!vertShader || !fragShader)
        return false;
    SPDLOG_INFO("vertex shader id: {}", vertShader->Get());
    SPDLOG_INFO("fragment shader id: {}", fragShader->Get());

    m_program = Program::Create({ fragShader, vertShader });
    if (!m_program)
        return false;
    SPDLOG_INFO("program id: {}", m_program->Get());
}


int Object::SetMVP(const glm::mat4& mvp) {
    m_mvp = mvp;
    return 1;
}

void Object::Render() {
    m_program->Use();

    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    m_program->SetUniform("transform", m_mvp);
}

Object::~Object() {
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
}
