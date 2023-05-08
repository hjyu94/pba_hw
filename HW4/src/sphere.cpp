#include "sphere.h"

SpherePtr Sphere::Create() {
    SpherePtr sphere = SpherePtr(new Sphere());
    if (!sphere->Init())
        return nullptr;
    return sphere;
}

void Sphere::Render(const Program* program, const View view_type) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, this->m_center);
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

    return true;
}

void Sphere::SetIntersected(const bool is_Intersected)
{
    m_isIntersected = is_Intersected;
}

const glm::vec3& Sphere::GetColor()
{
    return m_color;
}

const glm::vec3& Sphere::GetCenter()
{
    return m_center;
}

const float Sphere::GetRadius()
{
    return m_radius;
}

const AABBPtr Sphere::GetAABB()
{
    return m_AABB;
}

void Sphere::setRadius(const float radius)
{
    m_radius = radius;
}

void Sphere::setCenter(const glm::vec3& center)
{
    m_center = center;
    m_AABB = AABB::Create(
        m_center - glm::vec3(m_radius, m_radius, m_radius),
        m_center + glm::vec3(m_radius, m_radius, m_radius)
    );
}

Sphere::~Sphere()
{

}
