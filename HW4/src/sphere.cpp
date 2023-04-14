#include "sphere.h"

SpherePtr Sphere::Create() {
    SpherePtr sphere = SpherePtr(new Sphere());
    if (!sphere->Init())
        return nullptr;
    return sphere;
}

void Sphere::Render(const Program* program) {
    program->SetUniform("objectColor", this->m_color);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, this->m_center);
    program->SetUniform("model", model);

    m_model->Draw(program);
}

bool Sphere::Init() {
    // TODO
    m_model = Model::Load(getRelativePath("/model/sphere.obj"));

    if (!m_model)
        return false;

    return true;
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

Sphere::~Sphere()
{

}
