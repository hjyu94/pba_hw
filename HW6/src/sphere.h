#ifndef __SPHERE_H__
#define __SPHERE_H__

#include "common.h"
#include "model.h"
#include "AABB.h"
#include "rigidbody.h"

CLASS_PTR(Sphere);
class Sphere: public RigidBody {
public:
    static SpherePtr Create();
    void Render(const Program* program, const View view_type);
    bool Init();

    inline const glm::vec3& GetColor() { return m_color; }
    inline const glm::vec3& GetCenter() { return m_center; }
    inline const float GetRadius() { return m_radius; }
    inline const AABBPtr GetAABB() { return m_AABB; }

    inline void SetIntersected(const bool is_Intersected) { m_isIntersected = is_Intersected; }
    inline void setRadius(const float radius) { m_radius = radius; }
    void setCenter(const glm::vec3& center);
    
    friend std::ostream& operator << (std::ostream& out, const Sphere& sphere)
    {
        std::cout << "center: " << sphere.m_center.x << ", " << sphere.m_center.y << ", " << sphere.m_center.z
            << " / radius: " << sphere.m_radius;
        return out; // for chaining
    }

    ~Sphere() {}

    void move(const float timestep);

    glm::vec3 m_force;

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