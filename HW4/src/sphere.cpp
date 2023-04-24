#include "sphere.h"

SpherePtr Sphere::Create() {
    SpherePtr sphere = SpherePtr(new Sphere());
    if (!sphere->Init())
        return nullptr;
    return sphere;
}

void Sphere::Render(const Program* program, const View view_type) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(m_radius));
    model = glm::translate(model, this->m_center);
    program->SetUniform("model", model);

    // set color
    glm::vec3 objectColor;
    switch (view_type)
    {
    case View::VIEW_MODEL:
        objectColor = this->m_color;
        break;
    default:
        if (m_isIntersected)
            objectColor = glm::vec3(1.f, 0.f, 0.f);
        else
            objectColor = glm::vec3(0.f, 1.f, 0.f);
        break;
    }
    program->SetUniform("objectColor", objectColor);

    m_model->Draw(program);
}

bool Sphere::Init() {
    m_model = Model::Load(getRelativePath("/model/sphere.obj"));

    if (!m_model)
        return false;

    //m_center = glm::vec3(0.f, 0.f, 0.f);
    m_center = glm::vec3(
        getRandomFloat(-1.f, 1.f),
        getRandomFloat(-1.f, 1.f),
        getRandomFloat(-1.f, 1.f)
    );

    m_color = glm::vec3(
        getRandomFloat(0, 1),
        getRandomFloat(0, 1),
        getRandomFloat(0, 1)
    );

    //m_radius = 2.f;
    m_radius = getRandomFloat(0.5f, 3.0f);

    SPDLOG_INFO("center: {}, {}, {}", m_center.x, m_center.y, m_center.z);
    SPDLOG_INFO("radius: {}", m_radius);

    m_AABB = AABB::Create(m_center - glm::vec3(m_radius, m_radius, m_radius), 
        m_center + glm::vec3(m_radius, m_radius, m_radius));

    SPDLOG_INFO("AABB: {},{} / {},{} / {},{}", m_AABB->m_start_position.x, m_AABB->m_end_position.x, m_AABB->m_start_position.y, m_AABB->m_end_position.y, m_AABB->m_start_position.z, m_AABB->m_end_position.z);
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

Sphere::~Sphere()
{

}
