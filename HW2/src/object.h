#ifndef __OBJECT_H__
#define __OBJECT_H__

#include "common.h"
#include "program.h"

using namespace glm;
using namespace std;

CLASS_PTR(Object)
class Object {
public:
    static ObjectUPtr Create();
    ~Object();
    
    void Render();
    int SetMVP(const glm::mat4& mvp);

private:
    Object() {}
    bool Init();
    
    ProgramUPtr m_program;
    
    uint32_t m_vbo{ 0 };
    uint32_t m_vao{ 0 };
    uint32_t m_ebo{ 0 };

    std::vector<float> m_vertices;
    glm::mat4 m_mvp{ 1.0f };
};

#endif // !__OBJECT_H__
