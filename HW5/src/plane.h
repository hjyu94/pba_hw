#ifndef __PLANE_H__
#define __PLANE_H__

#include "common.h"
#include "program.h"
#include "model.h"

CLASS_PTR(Plane)
class Plane {
public:
    static PlaneUPtr Create();
    void Render(const Program* program);
    bool Init();
    ~Plane();

private:
    Plane() {};
    ModelUPtr m_model;
    glm::vec3 m_color{ glm::vec3(0.7f, 0.7f, .3f) };
};

#endif // __PLANE_H__