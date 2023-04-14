#ifndef __SPHERE_H__
#define __SPHERE_H__

#include "common.h"
#include "model.h"
#include <glm/glm.hpp>

CLASS_PTR(Sphere);
class Sphere {
public:
    static SpherePtr Create();
    void Render(const Program* program);
    bool Init();

    const glm::vec3& GetColor();
    const glm::vec3& GetCenter();
    const float GetRadius();

    ~Sphere();
private:
    Sphere() {}

    ModelUPtr m_model;

    glm::vec3 m_color{ glm::vec3(0.f, 0.f, 0.f) };
    glm::vec3 m_center{ glm::vec3(0.f, 0.f, 0.f) };
    float m_radius = 0.f;
};

#endif // __Sphere_H__