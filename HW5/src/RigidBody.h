#ifndef __RIGIDBODY_H__
#define __RIGIDBODY_H__

#include "common.h"

CLASS_PTR(RigidBody)
class RigidBody {
public:
    glm::vec3 m_velocity = glm::vec3{ 0.f, 0.f, 0.f };
    glm::vec3 m_momentum = glm::vec3{ 0.f, 0.f, 0.f };
    
    glm::vec3 m_angular_momentum = glm::vec3{ 0.f, 0.f, 0.f };
    glm::vec3 m_angular_vel = glm::vec3{ 0.f, 0.f, 0.f };
    
    glm::quat m_quaternion = glm::quat{ 0.f, 0.f, 0.f, 0.f };

    float m_mass = 0.f;
    float m_inertia = 0.f;

    float m_coefficient_of_restitution = 0.9f;
};

#endif