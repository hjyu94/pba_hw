#ifndef __CEILING_H__
#define __CEILING_H__

#include "common.h"
#include "program.h"

CLASS_PTR(Ceiling)
class Ceiling {
public:
    static CeilingUPtr Create();
    ~Ceiling();
    
    void Render();
    int SetMVP(const glm::mat4& mvp);

private:
    Ceiling() {}
    bool Init();
    
    ProgramUPtr m_program;
    
    uint32_t m_vbo{ 0 };
    uint32_t m_vao{ 0 };
    uint32_t m_ebo{ 0 };

    std::vector<float> m_vertices;
    glm::mat4 m_mvp{ 1.0f };
};

#endif // !__CEILING_H__
