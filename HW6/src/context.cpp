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

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        is_paused = is_paused ? false : true;

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

void Context::Update() 
{
    if (is_paused)
        return;

    // 1) collision detection, collision response
    // floor ~ sphere
    for (auto& sphere : m_spheres)
    {
        float v_rel = glm::dot(sphere->m_velocity, glm::vec3(0, 1, 0));
        bool is_close = sphere->GetCenter().y - sphere->GetRadius() < distance_epsilon
            && glm::abs(sphere->GetCenter().x) <= m_plane->m_x
            && glm::abs(sphere->GetCenter().z) <= m_plane->m_z;
        if (is_close && v_rel < - v_epsilon)
        {
            auto pa = glm::vec3(sphere->GetCenter().x, 0, sphere->GetCenter().z);
            auto padot = sphere->m_velocity + glm::cross(sphere->m_angular_vel, pa - sphere->GetCenter());
            auto n = glm::vec3(0, 1, 0);
            auto vrel = glm::dot(n, padot);
            auto ra = pa - sphere->GetCenter();
            auto j = -(1 + sphere->m_coefficient_of_restitution) * vrel /
                ((1 / sphere->m_mass) + glm::dot(n, (1 / sphere->m_inertia) * glm::cross(glm::cross(ra, n), ra)));

            sphere->m_momentum += j * n;
            sphere->m_velocity = sphere->m_momentum / sphere->m_mass;

            sphere->m_angular_momentum += glm::cross(ra, j * n);
            sphere->m_angular_vel = (1 / sphere->m_inertia) * sphere->m_angular_momentum;
        }
    }

    // sphere ~ sphere
    FindActuallyOverlappedSpheres();

    for (auto& pair : m_narrow_colliding_spheres)
    {
        auto s1 = pair.first;
        auto s2 = pair.second;

        auto p1 = s1->GetCenter() + s1->GetRadius() * glm::normalize(s2->GetCenter() - s1->GetCenter());
        auto p1dot = s1->m_velocity + glm::cross(s1->m_angular_vel, p1 - s1->GetCenter());

        auto p2 = s2->GetCenter() + s2->GetRadius() * glm::normalize(s1->GetCenter() - s2->GetCenter());
        auto p2dot = s2->m_velocity + glm::cross(s2->m_angular_vel, p2 - s2->GetCenter());
        
        auto n1 = glm::normalize(s1->GetCenter() - s2->GetCenter());
        auto n2 = glm::normalize(s2->GetCenter() - s1->GetCenter());

        auto ra1 = p1 - s1->GetCenter();
        auto ra2 = p2 - s2->GetCenter();

        // for s1
        {
            auto sphere = s1;
            auto vrel = glm::dot(n1, p1dot - p2dot);

            if (vrel < -v_epsilon)
            {
                auto j = -(1 + sphere->m_coefficient_of_restitution) * vrel /
                    ((1 / s1->m_mass) + (1 / s2->m_mass)
                        + glm::dot(n1, (1 / s1->m_inertia) * glm::cross(glm::cross(ra1, n1), ra1))
                        + glm::dot(n1, (1 / s2->m_inertia) * glm::cross(glm::cross(ra2, n1), ra2))
                        );

                sphere->m_momentum += j * n1;
                sphere->m_velocity = sphere->m_momentum / sphere->m_mass;

                sphere->m_angular_momentum += glm::cross(ra1, j * n1);
                sphere->m_angular_vel = (1 / sphere->m_inertia) * sphere->m_angular_momentum;
            }
        }

        // for s2
        {
            auto sphere = s2;
            auto vrel = glm::dot(n2, p2dot - p1dot);
            
            if (vrel < -v_epsilon)
            {
                auto j = -(1 + sphere->m_coefficient_of_restitution) * vrel /
                    ((1 / s1->m_mass) + (1 / s2->m_mass)
                        + glm::dot(n2, (1 / s1->m_inertia) * glm::cross(glm::cross(ra1, n2), ra1))
                        + glm::dot(n2, (1 / s2->m_inertia) * glm::cross(glm::cross(ra2, n2), ra2))
                        );

                sphere->m_momentum += j * n2;
                sphere->m_velocity = sphere->m_momentum / sphere->m_mass;

                sphere->m_angular_momentum += glm::cross(ra2, j * n2);
                sphere->m_angular_vel = (1 / sphere->m_inertia) * sphere->m_angular_momentum;
            }
        }
    }

    // 2) resting contact
    // floor ~ sphere
    for (auto& sphere : m_spheres)
    {
        float v_rel = glm::dot(sphere->m_velocity, glm::vec3(0, 1, 0));
        bool is_close = sphere->GetCenter().y - sphere->GetRadius() < distance_epsilon
            && glm::abs(sphere->GetCenter().x) <= m_plane->m_x
            && glm::abs(sphere->GetCenter().z) <= m_plane->m_z;
        if (is_close && abs(v_rel) < v_epsilon)
        {
            auto pa = glm::vec3(sphere->GetCenter().x, 0, sphere->GetCenter().z);
            auto padot = sphere->m_velocity + glm::cross(sphere->m_angular_vel, pa - sphere->GetCenter());
            auto n = glm::vec3(0, 1, 0);
            auto vrel = glm::dot(n, padot);
            auto ra = pa - sphere->GetCenter();
            auto j = - vrel / ((1 / sphere->m_mass) + glm::dot(n, (1 / sphere->m_inertia) * glm::cross(glm::cross(ra, n), ra)));

            sphere->m_momentum += j * n;
            sphere->m_velocity = sphere->m_momentum / sphere->m_mass;

            sphere->m_angular_momentum += glm::cross(ra, j * n);
            sphere->m_angular_vel = (1 / sphere->m_inertia) * sphere->m_angular_momentum;
        }
    }

    // sphere ~ sphere
    for (auto& pair : m_narrow_colliding_spheres)
    {
        auto s1 = pair.first;
        auto s2 = pair.second;

        auto p1 = s1->GetCenter() + s1->GetRadius() * glm::normalize(s2->GetCenter() - s1->GetCenter());
        auto p1dot = s1->m_velocity + glm::cross(s1->m_angular_vel, p1 - s1->GetCenter());

        auto p2 = s2->GetCenter() + s2->GetRadius() * glm::normalize(s1->GetCenter() - s2->GetCenter());
        auto p2dot = s2->m_velocity + glm::cross(s2->m_angular_vel, p2 - s2->GetCenter());

        auto n1 = glm::normalize(s1->GetCenter() - s2->GetCenter());
        auto n2 = glm::normalize(s2->GetCenter() - s1->GetCenter());

        auto ra1 = p1 - s1->GetCenter();
        auto ra2 = p2 - s2->GetCenter();

        // for s1
        {
            auto sphere = s1;
            auto vrel = glm::dot(n1, p1dot - p2dot);

            if (abs(vrel) < v_epsilon)
            {
                auto j = - vrel /
                    ((1 / s1->m_mass) + (1 / s2->m_mass)
                        + glm::dot(n1, (1 / s1->m_inertia) * glm::cross(glm::cross(ra1, n1), ra1))
                        + glm::dot(n1, (1 / s2->m_inertia) * glm::cross(glm::cross(ra2, n1), ra2))
                        );

                sphere->m_momentum += j * n1;
                sphere->m_velocity = sphere->m_momentum / sphere->m_mass;

                sphere->m_angular_momentum += glm::cross(ra1, j * n1);
                sphere->m_angular_vel = (1 / sphere->m_inertia) * sphere->m_angular_momentum;
            }
        }

        // for s2
        {
            auto sphere = s2;
            auto vrel = glm::dot(n2, p2dot - p1dot);

            if (abs(vrel) < v_epsilon)
            {
                auto j = - vrel /
                    ((1 / s1->m_mass) + (1 / s2->m_mass)
                        + glm::dot(n2, (1 / s1->m_inertia) * glm::cross(glm::cross(ra1, n2), ra1))
                        + glm::dot(n2, (1 / s2->m_inertia) * glm::cross(glm::cross(ra2, n2), ra2))
                        );

                sphere->m_momentum += j * n2;
                sphere->m_velocity = sphere->m_momentum / sphere->m_mass;

                sphere->m_angular_momentum += glm::cross(ra2, j * n2);
                sphere->m_angular_vel = (1 / sphere->m_inertia) * sphere->m_angular_momentum;
            }
        }
    }

    // 3) move
    for (auto& sphere : m_spheres)
    {
        glm::vec3 gravity = glm::vec3(0.f, m_gravity * sphere->m_mass, 0.f);
        sphere->m_force = gravity;
        sphere->move(m_timestep);
    }
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
                ComputeCollision();
            }
            ImGui::SameLine();

            if (ImGui::Button("--") && m_spheres.size() >= 1)
            {
                m_spheres.pop_back();
                ComputeCollision();
            }

            if (ImGui::Button("Recreate"))
            {
                const uint32_t count = m_spheres.size();
                m_spheres.clear();
                for (uint32_t i = 0; i < count; ++i)
                    m_spheres.push_back(Sphere::Create());
                
                ComputeCollision();
            }

            if (ImGui::Button("Camera reset"))
                ResetCamera();
        }

        ImGui::DragFloat("Time Step", &m_timestep, 0.005f, 0.f, 0.5f, "%.2f");
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

    m_program->SetUniform("objectColor", glm::vec3(0.3f, 0.4f, 1.f));
    m_plane->Render(m_program.get());
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

    m_spheres.push_back(Sphere::Create());

    m_plane = Plane::Create();


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
        SweepAndPrune();
        for (auto pair : m_broad_colliding_spheres)
        {
            pair.first->SetIntersected(true);
            pair.second->SetIntersected(true);
        }
        break;

    case View::VIEW_NARROW:
        FindActuallyOverlappedSpheres();
        for (auto pair : m_narrow_colliding_spheres)
        {
            pair.first->SetIntersected(true);
            pair.second->SetIntersected(true);
        }
        break;

    case View::VIEW_PENETRATION:
        FindActuallyOverlappedSpheres();
        for (auto pair : m_narrow_colliding_spheres)
        {
            pair.first->SetIntersected(true);
            pair.second->SetIntersected(true);
        }
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
}

void Context::FindActuallyOverlappedSpheres()
{
    // init
    m_narrow_colliding_spheres.clear();
    m_lines.clear();

    // get broad colliding pairs
    SweepAndPrune();

    // cacluate narrow colliding pairs
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
