#include "plane.h"

PlanePtr Plane::Create()
{
    auto plane = PlanePtr(new Plane());
    if (!plane->Init())
        return nullptr;
    return plane;
}

void Plane::Render(const Program* program)
{
    glm::mat4 model = glm::mat4(1.0f);
    program->SetUniform("model", model);

    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

bool Plane::Init()
{
    float vertices[] = {
        // point	        // normal
        +m_x, 0.0f, -m_z,   0.f, 1.f, 0.f, // top right
        +m_x, 0.0f, +m_z,   0.f, 1.f, 0.f, // bottom right
        -m_x, 0.0f, +m_z,   0.f, 1.f, 0.f, // bottom left
        -m_x, 0.0f, -m_z,   0.f, 1.f, 0.f // top left
    };

    unsigned int indices[] = {
        // note that we start from 0!
        0, 1, 3, // first Triangle
        1, 2, 3	 // second Triangle
    };

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // texture coord attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);

    m_mass = 0.f;
    m_inertia = 0.f;

    m_velocity = glm::vec3{ 0.f, 0.f, 0.f };
    m_momentum = glm::vec3{ 0.f, 0.f, 0.f };
    
    m_angular_momentum = glm::vec3{ 0.f, 0.f, 0.f };
    m_angular_vel = glm::vec3{ 0.f, 0.f, 0.f };

    return true;
}

