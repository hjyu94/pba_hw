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

using namespace std;

void Context::Update() {
    //mRigidSphere[0]->Update(mSimulationTime, mSimulationDeltaTime);
    //mRigidSphere[0]->UpdateWorldQuantities();

    DoCollisionDetection();
    DoCollisionResponse(mSimulationTime, mSimulationDeltaTime);
    mSimulationTime += mSimulationDeltaTime;

    //cout << mRigidSphere[0]->m_sphere->GetCenter().y << endl;
}

void Context::Render(GLFWwindow* window) 
{
    // start of ui
    if (ImGui::Begin("Mass Spring Simulation")) {
        // number of sphere
        {
            //ImGui::Text("number of sphere: %d", m_spheres.size());
            //ImGui::SameLine();

            //if (ImGui::Button("++") && m_spheres.size() <= 19)
            //{
            //    is_broad_computation_necessary = true;
            //    is_narrow_computation_necessary = true;
            //    
            //    m_spheres.push_back(Sphere::Create());
            //    ComputeCollision();
            //}
            //ImGui::SameLine();

            //if (ImGui::Button("--") && m_spheres.size() >= 1)
            //{
            //    is_broad_computation_necessary = true;
            //    is_narrow_computation_necessary = true;

            //    m_spheres.pop_back();
            //    ComputeCollision();
            //}

            //if (ImGui::Button("Recreate"))
            //{
            //    const uint32_t count = m_spheres.size();
            //    m_spheres.clear();
            //    for (uint32_t i = 0; i < count; ++i)
            //        m_spheres.push_back(Sphere::Create());
            //    
            //    is_broad_computation_necessary = true;
            //    is_narrow_computation_necessary = true;
            //    ComputeCollision();
            //}

            //if (ImGui::Button("Camera reset"))
            //    ResetCamera();

            //if (ImGui::Button("Recompute"))
            //{
            //    is_broad_computation_necessary = true;
            //    is_narrow_computation_necessary = true;
            //    ComputeCollision();
            //}

            //if (ImGui::Button("Log"))
            //{
            //    SPDLOG_INFO("Log button clicked.");
            //    
            //    SPDLOG_INFO("Spheres");
            //    for (const auto& e : m_spheres)
            //    {
            //        std::cout << *e << std::endl;
            //        std::cout << *e->GetAABB() << std::endl;
            //    }

            //    uint32_t i = 0;
            //    switch (static_cast<View>(m_view_type))
            //    {
            //    case View::VIEW_BROAD:
            //        SPDLOG_INFO("Intersected AABB");
            //        i = 0;
            //        for (const auto& e : m_broad_colliding_spheres)
            //            std::cout << i++ << ") " << *e.first->GetAABB() << std::endl << *e.second->GetAABB() << std::endl;
            //        break;

            //    case View::VIEW_NARROW:
            //    case View::VIEW_PENETRATION:
            //        SPDLOG_INFO("Narrow intersected");
            //        i = 0;
            //        for (const auto& e : m_narrow_colliding_spheres)
            //            std::cout << i++ << ") " << *e.first << ", " << *e.second << ", distance: " << e.first->GetRadius() + e.second->GetRadius() - glm::distance(e.first->GetCenter(), e.second->GetCenter()) << std::endl;

            //        i = 0;
            //        for (const auto& e : m_lines)
            //            std::cout << i++ << ")" << *e << std::endl;
            //        break;
            //    }
            //}
        }

        //// select view type
        //if (ImGui::RadioButton("Model View", &m_view_type, (int)View::VIEW_MODEL))
        //    ComputeCollision();
        //if (ImGui::RadioButton("Broad view", &m_view_type, (int)View::VIEW_BROAD))
        //    ComputeCollision();
        //if (ImGui::RadioButton("Narrow view", &m_view_type, (int)View::VIEW_NARROW))
        //    ComputeCollision();
        //if (ImGui::RadioButton("Penetration view", &m_view_type, (int)View::VIEW_PENETRATION))
        //    ComputeCollision();
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

        //for (auto& sphere : m_spheres)
        //{
        //    sphere->Render(m_program.get());
        //}
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
    
    mRigidSphere[0]->m_sphere->Render(m_program.get());
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



    mRigidSphere.push_back(std::make_shared<RigidSphere>(10000.f));
    initializeSpheres();


    //double radius{}, massDensity{};
    //Vector3<double> position{}, linearVelocity{}, angularVelocity{};
    //Quaternion<double> qOrientation{};

    ////3.0072975
    ////    1.0
    ////    - 11.6647 10.7203 31.5019
    ////    - 0.729045991406439 0.670017179989159 0.9377355422484628
    ////    - 0.6232360479056378 0.09444119273570384 - 0.3836658989859935 - 0.5579319140345902
    ////    0.00985762603835613 0.00992922651096017 0.00935389874021005

    //radius = 3.0;
    //massDensity = 1.0;
   

    /*SpherePtr sphere = Sphere::Create();
    sphere->setCenter(glm::vec3(0.f, 6.f, 0.f));
    m_spheres.push_back(sphere);*/


    mSimulationTime = 0.0f;
    mSimulationDeltaTime = 0.01f;

    return true;
}

//
//void Context::ComputeCollision()
//{
//    // init
//    for (auto sphere : m_spheres)
//        sphere->SetIntersected(false);
//
//    switch (static_cast<View>(m_view_type))
//    {
//    case View::VIEW_MODEL:
//        break;
//
//    case View::VIEW_BROAD:
//        if (is_broad_computation_necessary)
//            SweepAndPrune();
//    
//        for (auto pair : m_broad_colliding_spheres)
//        {
//            pair.first->SetIntersected(true);
//            pair.second->SetIntersected(true);
//        }
//        
//        is_broad_computation_necessary = false;
//        break;
//
//    case View::VIEW_NARROW:
//        if (is_narrow_computation_necessary)
//            FindActuallyOverlappedSpheres();
//
//        for (auto pair : m_narrow_colliding_spheres)
//        {
//            pair.first->SetIntersected(true);
//            pair.second->SetIntersected(true);
//        }
//
//        is_narrow_computation_necessary = false;
//        break;
//
//    case View::VIEW_PENETRATION:
//        if (is_narrow_computation_necessary)
//            FindActuallyOverlappedSpheres();
//
//        for (auto pair : m_narrow_colliding_spheres)
//        {
//            pair.first->SetIntersected(true);
//            pair.second->SetIntersected(true);
//        }
//
//        is_narrow_computation_necessary = false;
//        break;
//    }
//}
//
//void Context::SweepAndPrune()
//{
//    m_broad_colliding_spheres.clear();
//
//    std::vector<COLLIDING_SPHERE_PAIR> x_colliding_spheres;
//    std::vector<COLLIDING_SPHERE_PAIR> y_colliding_spheres;
//    std::vector<COLLIDING_SPHERE_PAIR> z_colliding_spheres;
//
//    // x-axis
//    {
//        std::vector<CompareHelper> helpers;
//
//        for (auto sphere : m_spheres)
//        {
//            helpers.push_back(CompareHelper(sphere, sphere->GetAABB()->m_start_position.x, true));
//            helpers.push_back(CompareHelper(sphere, sphere->GetAABB()->m_end_position.x, false));
//        }
//
//        sort(helpers.begin(), helpers.end(), CompareHelper::compare);
//        FindCollidingSpheres(helpers, x_colliding_spheres);
//    }
//
//    // y-axis
//    {
//        std::vector<CompareHelper> helpers;
//
//        for (auto sphere : m_spheres)
//        {
//            helpers.push_back(CompareHelper(sphere, sphere->GetAABB()->m_start_position.y, true));
//            helpers.push_back(CompareHelper(sphere, sphere->GetAABB()->m_end_position.y, false));
//        }
//
//        sort(helpers.begin(), helpers.end(), CompareHelper::compare);
//        FindCollidingSpheres(helpers, y_colliding_spheres);
//    }
//
//    // z-axis
//    {
//        std::vector<CompareHelper> helpers;
//
//        for (auto sphere : m_spheres)
//        {
//            helpers.push_back(CompareHelper(sphere, sphere->GetAABB()->m_start_position.z, true));
//            helpers.push_back(CompareHelper(sphere, sphere->GetAABB()->m_end_position.z, false));
//        }
//
//        sort(helpers.begin(), helpers.end(), CompareHelper::compare);
//        FindCollidingSpheres(helpers, z_colliding_spheres);
//    }
//    
//    auto temp_output = Intersection(x_colliding_spheres, y_colliding_spheres);
//    m_broad_colliding_spheres = Intersection(temp_output, z_colliding_spheres);
//
//    // Log
//    for (auto& e : m_broad_colliding_spheres)
//    {
//        std::cout << "[Intersected AABB]" << std::endl;
//        std::cout << *e.first->GetAABB() << std::endl << *e.second->GetAABB() << std::endl;
//    }
//}
//
//void Context::FindActuallyOverlappedSpheres()
//{
//    // init
//    m_narrow_colliding_spheres.clear();
//    m_lines.clear();
//
//    // get broad colliding pairs
//    SweepAndPrune();
//
//    // cacluate narrow colliding pairs
//    SPDLOG_INFO("penetration view log");
//    for (auto broad_pair : m_broad_colliding_spheres)
//    {
//        auto first_sphere = broad_pair.first;
//        auto second_sphere = broad_pair.second;
//        auto distance = glm::distance(first_sphere->GetCenter(), second_sphere->GetCenter());
//        auto radius_sum = first_sphere->GetRadius() + second_sphere->GetRadius();
//        if (distance < radius_sum) {
//            m_narrow_colliding_spheres.push_back(COLLIDING_SPHERE_PAIR(first_sphere, second_sphere));
//            
//            glm::vec3 ray = glm::normalize(second_sphere->GetCenter() - first_sphere->GetCenter());
//            
//            glm::vec3 penetrated_point1 = first_sphere->GetCenter() + first_sphere->GetRadius() * ray;
//            glm::vec3 penetrated_point2 = second_sphere->GetCenter() - second_sphere->GetRadius() * ray;
//
//            m_lines.push_back(Line::Create(penetrated_point1, penetrated_point2));
//            
//            SPDLOG_INFO("p1: {}, {}, {} / p2: {}, {}, {}", penetrated_point1.x, penetrated_point1.y, penetrated_point1.z, penetrated_point2.x, penetrated_point2.y, penetrated_point2.z);
//        }
//    }
//}
//
//void Context::FindCollidingSpheres(std::vector<CompareHelper> helpers, std::vector<COLLIDING_SPHERE_PAIR>& output)
//{
//    std::vector<CompareHelper> temp;
//    for (auto helper : helpers)
//    {
//        if (helper.m_is_start)
//        {
//            for (auto t : temp)
//            {
//                output.push_back(COLLIDING_SPHERE_PAIR(helper.m_spherePtr, t.m_spherePtr));
//            }
//            temp.push_back(helper);
//        }
//        else
//        {
//            temp.erase(remove(temp.begin(), temp.end(), helper), temp.end());
//        }
//    }
//}
//
//std::vector<COLLIDING_SPHERE_PAIR> Context::Intersection(const std::vector<COLLIDING_SPHERE_PAIR> v1, const std::vector<COLLIDING_SPHERE_PAIR> v2)
//{
//    std::vector<COLLIDING_SPHERE_PAIR> output;
//    for (auto v1_e : v1)
//    {
//        for (auto v2_e : v2)
//        {
//            if ((v1_e.first == v2_e.first && v1_e.second == v2_e.second)
//                || (v1_e.second == v2_e.first && v1_e.first == v2_e.second))
//            {
//                output.push_back(COLLIDING_SPHERE_PAIR(v1_e.first, v1_e.second));
//            }
//        }
//    }
//    return output;
//}

void Context::DoCollisionDetection()
{
    mContacts.clear();

    // Test for sphere-plane collisions. These checks are done in pairs with
    // the assumption that the sphere diameters are smaller than the distance
    // between parallel planar boundaries. In this case, only one of each
    // parallel pair of planes can be intersected at any time. Each pair of
    // parallel planes is tested in order to handle the case when a sphere
    // intersects two planes meeting at a region edge or three planes meeting
    // at a region corner. When the sphere is partially or fully outside a
    // plane, the interpenetration is removed to push the sphere back into
    // the simulation region.
    size_t const numSpheres = mRigidSphere.size();
    std::vector<bool> moved(numSpheres, false);
    double overlap{};
    for (size_t i = 0; i < numSpheres; ++i)
    {
        mRigidSphere[i]->UpdateWorldQuantities();
        auto const& sphere = mRigidSphere[i]->m_sphere;

        // Test for the sphere intersecting or occurring outside an
        // x-constant plane.
        overlap = sphere->GetRadius() - mRigidPlane[0]->GetSignedDistance(sphere->GetCenter());
        if (overlap > 0.0)
        {
            moved[i] = SetSpherePlaneContact(
                mRigidSphere[i], mRigidPlane[0], overlap);
        }
        else
        {
            overlap = sphere->GetRadius() - mRigidPlane[3]->GetSignedDistance(sphere->GetCenter());
            if (overlap > 0.0)
            {
                moved[i] = SetSpherePlaneContact(
                    mRigidSphere[i], mRigidPlane[3], overlap);
            }
        }

        // Test for the sphere intersection or occurring outside a
        // y-constant plane.
        overlap = sphere->GetRadius() - mRigidPlane[1]->GetSignedDistance(sphere->GetCenter());
        if (overlap > 0.0)
        {
            moved[i] = SetSpherePlaneContact(
                mRigidSphere[i], mRigidPlane[1], overlap);
        }
        else
        {
            overlap = sphere->GetRadius() - mRigidPlane[4]->GetSignedDistance(sphere->GetCenter());
            if (overlap > 0.0)
            {
                moved[i] = SetSpherePlaneContact(
                    mRigidSphere[i], mRigidPlane[4], overlap);
            }
        }

        // Test for the sphere intersecting or occurring outside a
        // z-constant plane.
        overlap = sphere->GetRadius() - mRigidPlane[2]->GetSignedDistance(sphere->GetCenter());
        if (overlap > 0.0)
        {
            moved[i] = SetSpherePlaneContact(
                mRigidSphere[i], mRigidPlane[2], overlap);
        }
        else
        {
            overlap = sphere->GetRadius() - mRigidPlane[5]->GetSignedDistance(sphere->GetCenter());
            if (overlap > 0.0)
            {
                moved[i] = SetSpherePlaneContact(
                    mRigidSphere[i], mRigidPlane[5], overlap);
            }
        }
    }

    // Test for sphere-sphere collisions.
    for (size_t i0 = 0; i0 + 1 < numSpheres; ++i0)
    {
        auto const& sphere0 = mRigidSphere[i0]->m_sphere;

        for (size_t i1 = i0 + 1; i1 < numSpheres; ++i1)
        {
            auto const& sphere1 = mRigidSphere[i1] -> m_sphere;

            // Test for overlap of sphere i0 and sphere i1.
            auto delta = sphere1->GetCenter() - sphere0->GetCenter();
            double lengthDelta = glm::length(delta);
            overlap = sphere0->GetRadius() + sphere1->GetRadius() - lengthDelta;
            if (overlap > 0.0)
            {
                UndoSphereOverlap(mRigidSphere[i0], mRigidSphere[i1],
                    overlap, moved[i0], moved[i1]);
            }
        }
    }
}

void Context::DoCollisionResponse(double time, double deltaTime)
{
    // Apply the instantaneous impulse forces at the current time.
    for (auto& contact : mContacts)
    {
        contact.ApplyImpulse();
    }

    for (size_t i = 0; i < mRigidSphere.size(); ++i)
    {
        // Solve the equations of motion.
        mRigidSphere[i]->Update(time, deltaTime);
    }
}

void Context::initializeSpheres()
{
    int i = 0;
    mRigidSphere[i]->Force = [this, i](double, RigidBodyState<double> const& state)
    {
        // The only external force is gravity.
        double constexpr gravityConstant = 9.81;   // m/sec^2
        Vector3<double> gravityDirection{ 0.0, -1.0, 0.0 };
        Vector3<double> gravityForce =
            (state.GetMass() * gravityConstant) * gravityDirection;

        // Take into account friction when the spheres are sliding on the
        // floor.
        double constexpr epsilon = 1e-03;
        Vector3<double> frictionForce{ 0.0, 0.0, 0.0 };
        double z = state.GetPosition()[2];
        double radius = mRigidSphere[i]->m_sphere->GetRadius();
        if (z - radius <= epsilon)
        {
            double constexpr viscosity = 1000.0;
            Vector3<double> linearVelocity = state.GetLinearVelocity();
            Normalize(linearVelocity);
            frictionForce = -viscosity * linearVelocity;
            frictionForce[2] = 0.0;
        }

        return gravityForce + frictionForce;
    };

    mRigidSphere[i]->Torque = [this, i](double, RigidBodyState<double> const& state)
    {
        // No external torque is applied. However, take into account friction
        // when the spheres are spinning on the floor.
        double constexpr epsilon = 1e-03;
        Vector3<double> torque{ 0.0, 0.0, 0.0 };
        double z = state.GetPosition()[2];
        double radius = mRigidSphere[i]->m_sphere->GetRadius();
        if (z - radius <= epsilon)
        {
            double constexpr viscosity = 1000.0;
            Vector3<double> angularVelocity = state.GetAngularVelocity();
            Normalize(angularVelocity);
            Vector3<double> newAngularVelocity = -viscosity * angularVelocity;
            Vector3<double> newAngularMomentum = state.GetWorldInertia() * newAngularVelocity;
            torque = newAngularMomentum;
        }
        return torque;
    };
}

void Context::ResetCamera()
{
    m_cameraPitch = 0.f;
    m_cameraYaw = 0.f;

    m_cameraFront = glm::vec3(0.0f, -1.0f, 0.0f);
    m_cameraPos = glm::vec3(0.0f, 0.0f, 20.0f);
    m_cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
}
