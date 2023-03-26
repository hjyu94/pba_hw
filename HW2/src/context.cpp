#include "context.h"
#include <imgui.h>
#include <iostream>

ContextUPtr Context::Create() {
    auto context = ContextUPtr(new Context());
    if (!context->Init())
        return nullptr;
    return std::move(context);
}

void Context::ProcessInput(GLFWwindow* window) {
    if (!m_cameraControl)
        return;
    const float cameraSpeed = 0.05f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        m_cameraPos += cameraSpeed * m_cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        m_cameraPos -= cameraSpeed * m_cameraFront;

    auto cameraRight = glm::normalize(glm::cross(m_cameraUp, -m_cameraFront));
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        m_cameraPos += cameraSpeed * cameraRight;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        m_cameraPos -= cameraSpeed * cameraRight;    

    auto cameraUp = glm::normalize(glm::cross(-m_cameraFront, cameraRight));
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        m_cameraPos += cameraSpeed * cameraUp;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        m_cameraPos -= cameraSpeed * cameraUp;

    std::cout << m_cameraPos.x << " " << m_cameraPos.y << " " << m_cameraPos.z << std::endl;
}

void Context::Reshape(int width, int height) {
    m_width = width;
    m_height = height;
    glViewport(0, 0, m_width, m_height);
}

void Context::MouseMove(double x, double y) {
    if (!m_cameraControl)
        return;
    auto pos = glm::vec2((float)x, (float)y);
    auto deltaPos = pos - m_prevMousePos;

    const float cameraRotSpeed = 0.8f;
    m_cameraYaw -= deltaPos.x * cameraRotSpeed;
    m_cameraPitch -= deltaPos.y * cameraRotSpeed;

    if (m_cameraYaw < 0.0f)   m_cameraYaw += 360.0f;
    if (m_cameraYaw > 360.0f) m_cameraYaw -= 360.0f;

    if (m_cameraPitch > 89.0f)  m_cameraPitch = 89.0f;
    if (m_cameraPitch < -89.0f) m_cameraPitch = -89.0f;

    m_prevMousePos = pos;    
}

void Context::MouseButton(int button, int action, double x, double y) {
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) {
            m_prevMousePos = glm::vec2((float)x, (float)y);
            m_cameraControl = true;
        }
        else if (action == GLFW_RELEASE) {
            m_cameraControl = false;
        }
    }
}

void Context::Render() {
    //if (ImGui::Begin("ui window")) {
    //    if (ImGui::ColorEdit4("clear color", glm::value_ptr(m_clearColor))) {
    //        glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);
    //    }
    //    ImGui::Separator();
    //    ImGui::DragFloat3("camera pos", glm::value_ptr(m_cameraPos), 0.01f);
    //    ImGui::DragFloat("camera yaw", &m_cameraYaw, 0.5f);
    //    ImGui::DragFloat("camera pitch", &m_cameraPitch, 0.5f, -89.0f, 89.0f);
    //    ImGui::Separator();
    //    if (ImGui::Button("reset camera")) {
    //        m_cameraYaw = 0.0f;
    //        m_cameraPitch = 0.0f;
    //        m_cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
    //    }
    //}
    //ImGui::End();

    //std::vector<glm::vec3> cubePositions = {
    //    glm::vec3( 0.0f, 0.0f, 0.0f),
    //    glm::vec3( 2.0f, 5.0f, -15.0f),
    //    glm::vec3(-1.5f, -2.2f, -2.5f),
    //    glm::vec3(-3.8f, -2.0f, -12.3f),
    //    glm::vec3( 2.4f, -0.4f, -3.5f),
    //    glm::vec3(-1.7f, 3.0f, -7.5f),
    //    glm::vec3( 1.3f, -2.0f, -2.5f),
    //    glm::vec3( 1.5f, 2.0f, -2.5f),
    //    glm::vec3( 1.5f, 0.2f, -1.5f),
    //    glm::vec3(-1.3f, 1.0f, -1.5f),
    //};

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 1. camera
    m_cameraFront =
        glm::rotate(
            glm::mat4(1.0f),
            glm::radians(m_cameraYaw),
            glm::vec3(0.0f, 1.0f, 0.0f)
        )
        * glm::rotate(
            glm::mat4(1.0f),
            glm::radians(m_cameraPitch),
            glm::vec3(1.0f, 0.0f, 0.0f)
        )
        * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);

    auto projection = glm::perspective(
        glm::radians(45.0f),
        (float)m_width / (float)m_height,
        0.01f,
        20.0f
    );

    auto view = glm::lookAt(
        m_cameraPos,
        m_cameraPos + m_cameraFront,
        m_cameraUp
    );

    // 1. Draw ceiling
    {
        m_program->Use();

        glBindVertexArray(m_ceiling_vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        //auto pos = glm::vec3(0.f, 1.f, 0.f);
        auto model = glm::mat4(1.0f);
        //model = glm::translate(model, glm::vec3(0.f, 0.4f, 0.f));
        //model = glm::rotate(model, glm::radians(90.f), glm::vec3(1.f, 0.f, 0.f));
        auto transform = projection * view * model;

        m_program->SetUniform("transform", transform);
    }

    // 2. Draw line
    {
        //glBegin(GL_LINE_LOOP);
        //glVertex3f(0.f, 0.f, 0.f);
        //glVertex3f(0.f, -1.f, 0.f);
        //glEnd();
        
    }
    //for (size_t i = 0; i < cubePositions.size(); i++){
    //    auto& pos = cubePositions[i];
    //    auto model = glm::translate(glm::mat4(1.0f), pos);
    //    auto transform = projection * view * model;
    //    m_program->SetUniform("transform", transform);
    //    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    //}
}

bool Context::Init() {
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
    
    glGenVertexArrays(1, &m_ceiling_vao);
    glGenBuffers(1, &m_ceiling_vbo);
    glGenBuffers(1, &m_ceiling_ebo);

    glBindVertexArray(m_ceiling_vao);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_ceiling_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ceiling_ebo);
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

    m_program = Program::Create({fragShader, vertShader});
    if (!m_program)
        return false;
    SPDLOG_INFO("program id: {}", m_program->Get());

    glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);
    glEnable(GL_DEPTH_TEST);

    m_program->Use();

    glm::vec4 vec(1.0f,  0.0f, 0.0f, 1.0f);
    auto trans = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 0.0f));
    auto rot = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    auto scale = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f));
    vec = trans * rot * scale * vec;
    SPDLOG_INFO("transformed vec: [{}, {}, {}]", vec.x, vec.y, vec.z);

    return true;
}
