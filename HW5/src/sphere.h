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
    void Render(const Program* program);
    bool Init();

    void SetIntersected(const bool is_Intersected);

    const glm::vec3& GetColor();
    const glm::vec3& GetCenter();
    const float GetRadius();
    const AABBPtr GetAABB();

    void setRadius(const float radius);
    void setCenter(const glm::vec3& center);

    friend std::ostream& operator << (std::ostream& out, const Sphere& sphere)
    {
        std::cout << "center: " << sphere.m_center.x << ", " << sphere.m_center.y << ", " << sphere.m_center.z
            << " / radius: " << sphere.m_radius;
        return out; // for chaining
    }

    ~Sphere();
private:
    Sphere() {}

    ModelUPtr m_model;

    glm::vec3 m_color{ glm::vec3(1.f, 1.f, 1.f) };
    glm::vec3 m_center{ glm::vec3(0.f, 0.f, 0.f) };
    float m_radius = 1.f;

    bool m_isIntersected = false;
    
    AABBPtr m_AABB;
    
};

#endif // __Sphere_H__