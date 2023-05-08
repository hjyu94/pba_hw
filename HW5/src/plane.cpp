#include "plane.h"

PlaneUPtr Plane::Create()
{
    auto Plane = PlaneUPtr(new Plane());
    if (!Plane->Init())
        return nullptr;

    return Plane;
}

void Plane::Render(const Program* program)
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.f, 0.f, 0.f));
    program->SetUniform("model", model);
    program->SetUniform("objectColor", m_color);
    m_model->Draw(program);
}

bool Plane::Init() {
    m_model = Model::Load(getRelativePath("/model/plane.obj"));

    if (!m_model)
        return false;

    return true;
}

Plane::~Plane()
{
}
