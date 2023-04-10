#include "context.h"
#include "image.h"
#include <imgui.h>

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
}

void Context::Reshape(int width, int height) {
    m_width = width;
    m_height = height;
    glViewport(0, 0, m_width, m_height);
}

void Context::MouseMove(double x, double y) {
    //if (!m_cameraControl)
    //    return;
    //auto pos = glm::vec2((float)x, (float)y);
    //auto deltaPos = pos - m_prevMousePos;

    //m_cameraYaw -= deltaPos.x * m_cameraRotSpeed;
    //m_cameraPitch -= deltaPos.y * m_cameraRotSpeed;

    //if (m_cameraYaw < 0.0f)   m_cameraYaw += 360.0f;
    //if (m_cameraYaw > 360.0f) m_cameraYaw -= 360.0f;

    //if (m_cameraPitch > 89.0f)  m_cameraPitch = 89.0f;
    //if (m_cameraPitch < -89.0f) m_cameraPitch = -89.0f;

    //m_prevMousePos = pos;    
}

void Context::MouseButton(int button, int action, double x, double y) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            m_prevMousePos = glm::vec2((float)x, (float)y);
            m_cameraControl = true;
        }
        else if (action == GLFW_RELEASE) {
            m_cameraControl = false;
        }
    }
}

void Context::Update() {

    //// solve ODE
    //if (std::string(m_current_method) == "Explicit Euler")
    //{
    //    // computation
    //    float current_distance = m_end_point.y;
    //    float current_acceleration = - m_ks / m_mass * (current_distance - m_rest_length) - m_kd / m_mass * m_current_velocity + m_gravity;
    //    float next_velocity = m_current_velocity + m_timestep * current_acceleration;
    //    float next_distance = current_distance + m_timestep * m_current_velocity;
    //    
    //    // setting
    //    m_end_point.y = next_distance;
    //    m_current_velocity = next_velocity;
    //}
    //else if (std::string(m_current_method) == "Symplectic Euler")
    //{
    //    // computation
    //    float current_distance = m_end_point.y;
    //    float current_acceleration = -m_ks / m_mass * (current_distance - m_rest_length) - m_kd / m_mass * m_current_velocity + m_gravity;
    //    float next_velocity = m_current_velocity + m_timestep * current_acceleration;
    //    float next_distance = current_distance + m_timestep * next_velocity;

    //    // setting
    //    m_end_point.y = next_distance;
    //    m_current_velocity = next_velocity;
    //}
    //else if (std::string(m_current_method) == "Implicit Euler") {
    //    // computation
    //    float dfdv = -m_kd / m_mass;
    //    float dfdx = -m_ks / m_mass;

    //    float current_distance = m_end_point.y;
    //    float current_acceleration =
    //            -m_ks / m_mass * (current_distance - m_rest_length) - m_kd / m_mass * m_current_velocity + m_gravity;

    //    float left = 1 - m_timestep * dfdv - m_timestep * m_timestep * dfdx;
    //    float right = m_timestep * (current_acceleration + m_timestep * dfdx * m_current_velocity);
    //    float delta_velocity = right / left;
    //    float delta_distance = m_timestep * (m_current_velocity + delta_velocity);

    //    float next_velocity = m_current_velocity + delta_velocity;
    //    float next_distance = current_distance + delta_distance;

    //    // setting
    //    m_end_point.y = next_distance;
    //    m_current_velocity = next_velocity;
    //}
}

void Context::Render(GLFWwindow* window) {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_cameraFront = glm::rotate(glm::mat4(1.0f), glm::radians(m_cameraYaw), glm::vec3(0.0f, 1.0f, 0.0f)) 
        * glm::rotate(glm::mat4(1.0f), glm::radians(m_cameraPitch), glm::vec3(1.0f, 0.0f, 0.0f)) 
        * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);

    auto projection = glm::perspective(
        glm::radians(45.0f),
        (float)m_width / (float)m_height, 
        0.01f, 
        100.0f
    );
    
    auto view = glm::lookAt(
        m_cameraPos,
        m_cameraPos + m_cameraFront,
        m_cameraUp
    );
    
    m_program->Use();

    m_program->SetUniform("view", view);
    m_program->SetUniform("projection", projection);
    
    m_program->SetUniform("lightPos", m_lightPos);
    m_program->SetUniform("viewPos", m_cameraPos);
    m_program->SetUniform("lightColor", glm::vec3(1.f, 1.f, 1.f));

    
    // wire
    {
        m_program->SetUniform("objectColor", glm::vec3(1.f, 1.f, 1.f));

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(90.f), glm::vec3(1.f, 0.f, 0.f));
        m_program->SetUniform("model", model);

        m_wire->Draw(m_program.get());
    }

    // sphere
    {
        m_program->SetUniform("objectColor", glm::vec3(.5f, 1.0f, 0.3f));
    
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
        m_program->SetUniform("model", model);
        
        m_bead->Draw(m_program.get());
    }
}

bool Context::Init() {
    glEnable(GL_DEPTH_TEST);
    glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);

    m_program = Program::Create(
        getRelativePath("/shader/lighting.vs"),
        getRelativePath("/shader/lighting.fs")
    );

    if (!m_program)
        return false;

    SPDLOG_INFO("program id: {}", m_program->Get());
    
    m_wire = Model::Load(std::string(CURRENT_SOURCE_DIR) + std::string("/model/wire.obj"));
    m_bead = Model::Load(std::string(CURRENT_SOURCE_DIR) + std::string("/model/sphere.obj"));

    if (!m_bead)
        return false;
    

    // environment parameter init
    this->InitializeEnvParameter();
    //SPDLOG_INFO("default solver: {}", m_current_method);

    return true;
}

void Context::InitializeEnvParameter() {
    /*m_elapsed_time = 0.f;
    m_start_point = glm::vec3(0.f);
    m_end_point = glm::vec3(0.f, m_start_length, 0.f);*/
}
