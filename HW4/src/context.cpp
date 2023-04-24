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
    if (!m_cameraControl)
        return;
    auto pos = glm::vec2((float)x, (float)y);
    auto deltaPos = pos - m_prevMousePos;

    m_cameraYaw -= deltaPos.x * m_cameraRotSpeed;
    m_cameraPitch -= deltaPos.y * m_cameraRotSpeed;

    if (m_cameraYaw < 0.0f)   m_cameraYaw += 360.0f;
    if (m_cameraYaw > 360.0f) m_cameraYaw -= 360.0f;

    if (m_cameraPitch > 89.0f)  m_cameraPitch = 89.0f;
    if (m_cameraPitch < -89.0f) m_cameraPitch = -89.0f;

    m_prevMousePos = pos;
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
    // do nothing
}

void Context::Render(GLFWwindow* window) 
{
    // start of ui
    if (ImGui::Begin("Mass Spring Simulation")) {
        // number of sphere
        {
            ImGui::Text("number of sphere: %d", m_spheres.size());
            ImGui::SameLine();

            if (ImGui::Button("++") && m_spheres.size() <= 19)
            {
                m_spheres.push_back(Sphere::Create());
                //ComputeCollision();
            }
            ImGui::SameLine();

            if (ImGui::Button("--") && m_spheres.size() >= 1)
            {
                m_spheres.pop_back();
                //ComputeCollision();
            }

            if (ImGui::Button("compute"))
            {
                ComputeCollision();
            }

            if (ImGui::Button("reset"))
            {
                for (auto sphere : m_spheres)
                    sphere->SetIntersected(false);
            }
        }

        // select view type
        ImGui::RadioButton("Model View", &m_view_type, (int)View::VIEW_MODEL);
        ImGui::RadioButton("Broad view", &m_view_type, (int)View::VIEW_BROAD);
        ImGui::RadioButton("Narrow view", &m_view_type, (int)View::VIEW_NARROW);
        ImGui::RadioButton("Penetration view", &m_view_type, (int)View::VIEW_PENETRATION);
    }
    ImGui::End();
    // end of ui

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
        sphere->Render(m_program.get(), static_cast<View>(m_view_type));
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


void Context::ComputeCollision()
{
    switch (static_cast<View>(m_view_type))
    {
    case View::VIEW_MODEL:
        break;
    case View::VIEW_BROAD:
        SweepAndPrune();
        for (auto pair : m_colliding_spheres)
        {
            pair.first->SetIntersected(true);
            pair.second->SetIntersected(true);
        }
        break;
    }
}

void Context::SweepAndPrune()
{
    m_colliding_spheres.clear();

    std::vector<COLLIDING_PAIR> x_colliding_spheres;
    std::vector<COLLIDING_PAIR> y_colliding_spheres;
    std::vector<COLLIDING_PAIR> z_colliding_spheres;

    int i = 0;
    for (auto e : m_spheres)
    {
        SPDLOG_INFO("{}¹øÂ° °ø:", i);
        SPDLOG_INFO("{}, {}, {}", e->GetAABB()->m_start_position.x, e->GetAABB()->m_start_position.y, e->GetAABB()->m_start_position.z);
        SPDLOG_INFO("{}, {}, {}", e->GetAABB()->m_end_position.x, e->GetAABB()->m_end_position.y, e->GetAABB()->m_end_position.z);
        i++;
    }

    // x-axis
    {
        std::vector<CompareHelper> helpers;

        for (auto sphere : m_spheres)
        {
            helpers.push_back(CompareHelper(sphere, sphere->GetAABB()->m_start_position.x, true));
            helpers.push_back(CompareHelper(sphere, sphere->GetAABB()->m_end_position.x, false));
        }

        sort(helpers.begin(), helpers.end(), CompareHelper::compare);
        FindCollidingSpheres(helpers, x_colliding_spheres);
    }

    // y-axis
    {
        std::vector<CompareHelper> helpers;

        for (auto sphere : m_spheres)
        {
            helpers.push_back(CompareHelper(sphere, sphere->GetAABB()->m_start_position.y, true));
            helpers.push_back(CompareHelper(sphere, sphere->GetAABB()->m_end_position.y, false));
        }

        sort(helpers.begin(), helpers.end(), CompareHelper::compare);
        FindCollidingSpheres(helpers, y_colliding_spheres);
    }

    // z-axis
    {
        std::vector<CompareHelper> helpers;

        for (auto sphere : m_spheres)
        {
            helpers.push_back(CompareHelper(sphere, sphere->GetAABB()->m_start_position.z, true));
            helpers.push_back(CompareHelper(sphere, sphere->GetAABB()->m_end_position.z, false));
        }

        sort(helpers.begin(), helpers.end(), CompareHelper::compare);
        FindCollidingSpheres(helpers, z_colliding_spheres);
    }
    
    Intersection(x_colliding_spheres, y_colliding_spheres, m_colliding_spheres);
    Intersection(m_colliding_spheres, z_colliding_spheres, m_colliding_spheres);
}

void Context::FindCollidingSpheres(std::vector<CompareHelper> helpers, std::vector<COLLIDING_PAIR>& output)
{
    std::vector<CompareHelper> temp;
    for (auto helper : helpers)
    {
        if (helper.m_is_start)
        {
            for (auto t : temp)
            {
                output.push_back(COLLIDING_PAIR(helper.m_spherePtr, t.m_spherePtr));
            }
            temp.push_back(helper);
        }
        else
        {
            temp.erase(remove(temp.begin(), temp.end(), helper), temp.end());
        }
    }
}

void Context::Intersection(std::vector<COLLIDING_PAIR> v1, std::vector<COLLIDING_PAIR> v2, std::vector<COLLIDING_PAIR>& output) {
    for (auto v1_e : v1)
    {
        for (auto v2_e : v2)
        {
            if ((v1_e.first == v2_e.first && v1_e.second == v2_e.second)
                || (v1_e.second == v2_e.first && v1_e.first == v2_e.second))
            {
                output.push_back(COLLIDING_PAIR(v1_e.first, v1_e.second));
            }
        }
    }
}
