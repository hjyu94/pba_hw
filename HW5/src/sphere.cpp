#include "sphere.h"
#include "math.h"

SpherePtr Sphere::Create() {
    SpherePtr sphere = SpherePtr(new Sphere());
    if (!sphere->Init())
        return nullptr;
    return sphere;
}

void Sphere::Render(const Program* program, const View view_type) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, this->m_center);
    
    m_quaternion = glm::normalize(m_quaternion);
    auto axis = glm::axis(m_quaternion);
    auto angle = glm::angle(m_quaternion);
    model = glm::rotate(model, angle, axis);

    model = glm::scale(model, glm::vec3(m_radius));
    program->SetUniform("model", model);
    
    // set color
    glm::vec3 objectColor;
    switch (view_type)
    {
    case View::VIEW_MODEL:
        objectColor = this->m_color;
        break;

    case View::VIEW_BROAD:
    case View::VIEW_NARROW:
        if (m_isIntersected)
            objectColor = glm::vec3(1.f, 0.f, 0.f);
        else
            objectColor = glm::vec3(0.f, 1.f, 0.f);
        break;

    case View::VIEW_PENETRATION:
        break;
    }
    program->SetUniform("objectColor", objectColor);

    if(view_type != View::VIEW_PENETRATION)
        m_model->Draw(program);
}

bool Sphere::Init() {
    m_model = Model::Load(getRelativePath("/model/sphere.obj"));

    if (!m_model)
        return false;

    m_center = glm::vec3(
        getRandomFloat(-3.f, 3.f),
        getRandomFloat(-3.f, 3.f),
        getRandomFloat(-3.f, 3.f)
    );

    m_color = glm::vec3(
        getRandomFloat(0, 1),
        getRandomFloat(0, 1),
        getRandomFloat(0, 1)
    );

    m_radius = getRandomFloat(0.5f, 1.0f);

    m_AABB = AABB::Create(
        m_center - glm::vec3(m_radius, m_radius, m_radius), 
        m_center + glm::vec3(m_radius, m_radius, m_radius)
    );

    SPDLOG_INFO("sphere created.");
    std::cout << *this << std::endl;
    std::cout << *m_AABB << std::endl;

    m_mass = 1.f; // getRandomFloat(3.f, 5.f)
    m_inertia = 0.4 * m_mass * m_radius * m_radius;

    m_velocity = glm::vec3{ 1.f, 0.f, 0.f };
    //m_velocity = glm::vec3{ 0.f, 0.f, 0.f };
    m_momentum = m_mass * m_velocity;

    m_angular_vel = glm::vec3{ 6.f, 6.f, 0.f };
    //m_angular_vel = glm::vec3{ 0.f, 0.f, 0.f };
    m_angular_momentum = m_inertia * m_angular_vel;

    m_quaternion = glm::angleAxis(0.f, glm::normalize(m_angular_vel));
    //m_quaternion = glm::quat(1.f, 0.f, 0.f, 0.f);
 
    return true;
}

void Sphere::setCenter(const glm::vec3& center)
{
    m_center = center;
    m_AABB = AABB::Create(
        m_center - glm::vec3(m_radius, m_radius, m_radius),
        m_center + glm::vec3(m_radius, m_radius, m_radius)
    );
}

void Sphere::move(const glm::vec3 force, const float timestep)
{
    m_momentum += timestep * force;
    m_angular_vel = (1 / m_inertia) * m_angular_momentum;
    m_velocity = m_momentum / m_mass;
    setCenter(m_center + timestep * m_velocity);
    m_quaternion += timestep * 0.5f * glm::angleAxis(glm::radians(180.f), m_angular_vel) * m_quaternion;
}
