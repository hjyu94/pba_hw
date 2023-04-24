#include "line.h"

LinePtr Line::Create(const glm::vec3 start, const glm::vec3 end)
{
    auto s = new Line();
    LinePtr Line = LinePtr(s);
    if (!Line->Init(start, end))
        return nullptr;
    return Line;
}

void Line::Render(const Program* program) {
    program->SetUniform("objectColor", glm::vec3(1.f, 0.f, 1.f));
    program->SetUniform("model", glm::mat4(1.0f));

    //// set up vertex data (and buffer(s)) and configure vertex attributes
    //// ------------------------------------------------------------------
    float vertices[] = {
        m_startPoint.x, m_startPoint.y, m_startPoint.z,
        m_endPoint.x, m_endPoint.y, m_endPoint.z
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);

    glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
    glDrawArrays(GL_LINES, 0, 2);
}

bool Line::Init(const glm::vec3 start, const glm::vec3 end) {
    m_startPoint = start;
    m_endPoint = end;

    return true;
}


Line::~Line()
{

}
