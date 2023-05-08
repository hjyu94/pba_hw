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
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        m_cameraPos += m_cameraMovSpeed * m_cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        m_cameraPos -= m_cameraMovSpeed * m_cameraFront;

    auto cameraRight = glm::normalize(glm::cross(m_cameraUp, -m_cameraFront));
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        m_cameraPos += m_cameraMovSpeed * cameraRight;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        m_cameraPos -= m_cameraMovSpeed * cameraRight;

    auto cameraUp = glm::normalize(glm::cross(-m_cameraFront, cameraRight));
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        m_cameraPos += m_cameraMovSpeed * cameraUp;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        m_cameraPos -= m_cameraMovSpeed * cameraUp;
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
                is_broad_computation_necessary = true;
                is_narrow_computation_necessary = true;
                
                m_spheres.push_back(Sphere::Create());
                ComputeCollision();
            }
            ImGui::SameLine();

            if (ImGui::Button("--") && m_spheres.size() >= 1)
            {
                is_broad_computation_necessary = true;
                is_narrow_computation_necessary = true;

                m_spheres.pop_back();
                ComputeCollision();
            }

            if (ImGui::Button("Recreate"))
            {
                const uint32_t count = m_spheres.size();
                m_spheres.clear();
                for (uint32_t i = 0; i < count; ++i)
                    m_spheres.push_back(Sphere::Create());
                
                is_broad_computation_necessary = true;
                is_narrow_computation_necessary = true;
                ComputeCollision();
            }

            if (ImGui::Button("Camera reset"))
                ResetCamera();

            if (ImGui::Button("Recompute"))
            {
                is_broad_computation_necessary = true;
                is_narrow_computation_necessary = true;
                ComputeCollision();
            }

            if (ImGui::Button("Log"))
            {
                SPDLOG_INFO("Log button clicked.");
                
                SPDLOG_INFO("Spheres");
                for (const auto& e : m_spheres)
                {
                    std::cout << *e << std::endl;
                    std::cout << *e->GetAABB() << std::endl;
                }

                uint32_t i = 0;
                switch (static_cast<View>(m_view_type))
                {
                case View::VIEW_BROAD:
                    SPDLOG_INFO("Intersected AABB");
                    i = 0;
                    for (const auto& e : m_broad_colliding_spheres)
                        std::cout << i++ << ") " << *e.first->GetAABB() << std::endl << *e.second->GetAABB() << std::endl;
                    break;

                case View::VIEW_NARROW:
                case View::VIEW_PENETRATION:
                    SPDLOG_INFO("Narrow intersected");
                    i = 0;
                    for (const auto& e : m_narrow_colliding_spheres)
                        std::cout << i++ << ") " << *e.first << ", " << *e.second << ", distance: " << e.first->GetRadius() + e.second->GetRadius() - glm::distance(e.first->GetCenter(), e.second->GetCenter()) << std::endl;

                    i = 0;
                    for (const auto& e : m_lines)
                        std::cout << i++ << ")" << *e << std::endl;
                    break;
                }
            }
        }

        // select view type
        if (ImGui::RadioButton("Model View", &m_view_type, (int)View::VIEW_MODEL))
            ComputeCollision();
        if (ImGui::RadioButton("Broad view", &m_view_type, (int)View::VIEW_BROAD))
            ComputeCollision();
        if (ImGui::RadioButton("Narrow view", &m_view_type, (int)View::VIEW_NARROW))
            ComputeCollision();
        if (ImGui::RadioButton("Penetration view", &m_view_type, (int)View::VIEW_PENETRATION))
            ComputeCollision();
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
    
    
    switch (static_cast<View>(m_view_type))
    {
    case View::VIEW_MODEL:
    case View::VIEW_NARROW:
    case View::VIEW_BROAD:
        m_program->Use();

        m_program->SetUniform("view", view);
        m_program->SetUniform("projection", projection);

        m_program->SetUniform("lightPos", m_lightPos);
        m_program->SetUniform("viewPos", m_cameraPos);
        m_program->SetUniform("lightColor", glm::vec3(1.f, 1.f, 1.f));

        for (auto& sphere : m_spheres)
        {
            sphere->Render(m_program.get(), static_cast<View>(m_view_type));
        }
        break;

    case View::VIEW_PENETRATION:
        m_line_program->Use();

        m_line_program->SetUniform("view", view);
        m_line_program->SetUniform("projection", projection);

        m_line_program->SetUniform("lightPos", m_lightPos);
        m_line_program->SetUniform("viewPos", m_cameraPos);
        m_line_program->SetUniform("lightColor", glm::vec3(1.f, 1.f, 1.f));

        for (auto& line : m_lines)
        {
            line->Render(m_line_program.get());
        }
        break;
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

    m_line_program = Program::Create(
        getRelativePath("/shader/line.vs"),
        getRelativePath("/shader/line.fs")
    );

    if (!m_line_program)
        return false;

    SPDLOG_INFO("program id: {}", m_line_program->Get());

    {
        SpherePtr sphere = Sphere::Create();
        sphere->setCenter(glm::vec3(-1.0, -3.0, 1.2));
        sphere->setRadius(0.7);
        m_spheres.push_back(sphere);
    }
    {
        SpherePtr sphere = Sphere::Create();
        sphere->setCenter(glm::vec3(-1.4, -2.8, -1.1));
        sphere->setRadius(0.6);
        m_spheres.push_back(sphere);
    }

    return true;
}


void Context::ComputeCollision()
{
    // init
    for (auto sphere : m_spheres)
        sphere->SetIntersected(false);

    switch (static_cast<View>(m_view_type))
    {
    case View::VIEW_MODEL:
        break;

    case View::VIEW_BROAD:
        if (is_broad_computation_necessary)
            SweepAndPrune();
    
        for (auto pair : m_broad_colliding_spheres)
        {
            pair.first->SetIntersected(true);
            pair.second->SetIntersected(true);
        }
        
        is_broad_computation_necessary = false;
        break;

    case View::VIEW_NARROW:
        if (is_narrow_computation_necessary)
            FindActuallyOverlappedSpheres();

        for (auto pair : m_narrow_colliding_spheres)
        {
            pair.first->SetIntersected(true);
            pair.second->SetIntersected(true);
        }

        is_narrow_computation_necessary = false;
        break;

    case View::VIEW_PENETRATION:
        if (is_narrow_computation_necessary)
            FindActuallyOverlappedSpheres();

        for (auto pair : m_narrow_colliding_spheres)
        {
            pair.first->SetIntersected(true);
            pair.second->SetIntersected(true);
        }

        is_narrow_computation_necessary = false;
        break;
    }
}

void Context::SweepAndPrune()
{
    m_broad_colliding_spheres.clear();

    std::vector<COLLIDING_SPHERE_PAIR> x_colliding_spheres;
    std::vector<COLLIDING_SPHERE_PAIR> y_colliding_spheres;
    std::vector<COLLIDING_SPHERE_PAIR> z_colliding_spheres;

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
    
    auto temp_output = Intersection(x_colliding_spheres, y_colliding_spheres);
    m_broad_colliding_spheres = Intersection(temp_output, z_colliding_spheres);

    // Log
    for (auto& e : m_broad_colliding_spheres)
    {
        std::cout << "[Intersected AABB]" << std::endl;
        std::cout << *e.first->GetAABB() << std::endl << *e.second->GetAABB() << std::endl;
    }
}

void Context::FindActuallyOverlappedSpheres()
{
    // init
    m_narrow_colliding_spheres.clear();
    m_lines.clear();

    // get broad colliding pairs
    SweepAndPrune();

    // cacluate narrow colliding pairs
    SPDLOG_INFO("penetration view log");
    for (auto broad_pair : m_broad_colliding_spheres)
    {
        auto first_sphere = broad_pair.first;
        auto second_sphere = broad_pair.second;
        auto distance = glm::distance(first_sphere->GetCenter(), second_sphere->GetCenter());
        auto radius_sum = first_sphere->GetRadius() + second_sphere->GetRadius();
        if (distance < radius_sum) {
            m_narrow_colliding_spheres.push_back(COLLIDING_SPHERE_PAIR(first_sphere, second_sphere));
            
            glm::vec3 ray = glm::normalize(second_sphere->GetCenter() - first_sphere->GetCenter());
            
            glm::vec3 penetrated_point1 = first_sphere->GetCenter() + first_sphere->GetRadius() * ray;
            glm::vec3 penetrated_point2 = second_sphere->GetCenter() - second_sphere->GetRadius() * ray;

            m_lines.push_back(Line::Create(penetrated_point1, penetrated_point2));
            
            SPDLOG_INFO("p1: {}, {}, {} / p2: {}, {}, {}", penetrated_point1.x, penetrated_point1.y, penetrated_point1.z, penetrated_point2.x, penetrated_point2.y, penetrated_point2.z);
        }
    }
}

void Context::FindCollidingSpheres(std::vector<CompareHelper> helpers, std::vector<COLLIDING_SPHERE_PAIR>& output)
{
    std::vector<CompareHelper> temp;
    for (auto helper : helpers)
    {
        if (helper.m_is_start)
        {
            for (auto t : temp)
            {
                output.push_back(COLLIDING_SPHERE_PAIR(helper.m_spherePtr, t.m_spherePtr));
            }
            temp.push_back(helper);
        }
        else
        {
            temp.erase(remove(temp.begin(), temp.end(), helper), temp.end());
        }
    }
}

std::vector<COLLIDING_SPHERE_PAIR> Context::Intersection(const std::vector<COLLIDING_SPHERE_PAIR> v1, const std::vector<COLLIDING_SPHERE_PAIR> v2)
{
    std::vector<COLLIDING_SPHERE_PAIR> output;
    for (auto v1_e : v1)
    {
        for (auto v2_e : v2)
        {
            if ((v1_e.first == v2_e.first && v1_e.second == v2_e.second)
                || (v1_e.second == v2_e.first && v1_e.first == v2_e.second))
            {
                output.push_back(COLLIDING_SPHERE_PAIR(v1_e.first, v1_e.second));
            }
        }
    }
    return output;
}

void Context::ResetCamera()
{
    m_cameraPitch = 0.f;
    m_cameraYaw = 0.f;

    m_cameraFront = glm::vec3(0.0f, -1.0f, 0.0f);
    m_cameraPos = glm::vec3(0.0f, 0.0f, 20.0f);
    m_cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
}
