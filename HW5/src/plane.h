#ifndef __PLANE_H__
#define __PLANE_H__

#include "common.h"
#include "program.h"
#include "rigidbody.h"

CLASS_PTR(Plane);
class Plane: public RigidBody {
public:
    static PlanePtr Create();
    void Render(const Program* program);
    bool Init();
    
    float m_x = 10.f;
    float m_z = 10.f;

private:
    Plane() {}

    uint32_t m_vbo{ 0 };
    uint32_t m_vao{ 0 };
    uint32_t m_ebo{ 0 };
};

#endif // __PLANE_H__