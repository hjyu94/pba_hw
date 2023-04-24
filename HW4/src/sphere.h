#ifndef __SPHERE_H__
#define __SPHERE_H__

#include "common.h"
#include "model.h"
#include "AABB.h"
#include <glm/glm.hpp>

CLASS_PTR(Sphere);
class Sphere {
public:
    static SpherePtr Create();
    void Render(const Program* program, const View view_type);
    bool Init();

    void SetIntersected(const bool is_Intersected);

    const glm::vec3& GetColor();
    const glm::vec3& GetCenter();
    const float GetRadius();
    const AABBPtr GetAABB();

    ~Sphere();
private:
    Sphere() {}

    ModelUPtr m_model;

    glm::vec3 m_color{ glm::vec3(0.5f, 0.5f, 1.f) };
    glm::vec3 m_center{ glm::vec3(1.f, 0.f, 0.f) };
    float m_radius = 0.f;

    bool m_isIntersected = false;
    
    AABBPtr m_AABB;
    
};

#endif // __Sphere_H__