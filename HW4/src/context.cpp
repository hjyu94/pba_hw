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
    //if (!m_cameraControl)
    //    return;
    //const float cameraSpeed = 0.05f;
    //if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    //    m_cameraPos += cameraSpeed * m_cameraFront;
    //if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    //    m_cameraPos -= cameraSpeed * m_cameraFront;

    //auto cameraRight = glm::normalize(glm::cross(m_cameraUp, -m_cameraFront));
    //if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    //    m_cameraPos += cameraSpeed * cameraRight;
    //if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    //    m_cameraPos -= cameraSpeed * cameraRight;    

    //auto cameraUp = glm::normalize(glm::cross(-m_cameraFront, cameraRight));
    //if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    //    m_cameraPos += cameraSpeed * cameraUp;
    //if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    //    m_cameraPos -= cameraSpeed * cameraUp;
}

void Context::Reshape(int width, int height) {
    m_width = width;
    m_height = height;
    glViewport(0, 0, m_width, m_height);
}

void Context::MouseMove(double x, double y) {
    //if (!m_cameraControl)
    //{
    //    m_mouse_force = glm::vec3(0.f, 0.f, 0.f);
    //    return;
    //}
 
    //auto pos = glm::vec2((float)x, (float)y);
    //auto deltaPos = pos - m_prevMousePos;
    //
    //m_mouse_force = m_mouse_force_scale * glm::vec3(deltaPos.x, -deltaPos.y, 0.f);
    //SPDLOG_INFO("m_mouse_force: {}, {}", m_mouse_force.x, m_mouse_force.y);
}

void Context::MouseButton(int button, int action, double x, double y) {
    //if (button == GLFW_MOUSE_BUTTON_LEFT) {
    //    if (action == GLFW_PRESS) {
    //        m_prevMousePos = glm::vec2((float)x, (float)y);
    //        m_cameraControl = true;
    //    }
    //    else if (action == GLFW_RELEASE) {
    //        m_cameraControl = false;
    //    }
    //}
}

void Context::Update() {

}

void Context::Render(GLFWwindow* window) 
{
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

    // sphere
    for (auto& sphere : m_spheres)
    {
        sphere->Render(m_program.get());
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
    
    SpherePtr sphere = Sphere::Create();
    m_spheres.push_back(sphere);
    

    return true;
}

