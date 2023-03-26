#ifndef __MASS_H__
#define __MASS_H__

#include "common.h"
#include "program.h"

CLASS_PTR(Mass)
class Mass {
public:
    static MassUPtr Create();
    ~Mass();
    
    void Render();
    int SetMVP(const glm::mat4& mvp);

private:
    Mass() {}
    bool Init();
    
    ProgramUPtr m_program;
    
    uint32_t m_vbo{ 0 };
    uint32_t m_vao{ 0 };
    uint32_t m_ebo{ 0 };

    std::vector<float> m_vertices;
    glm::mat4 m_mvp{ 1.0f };
};

#endif // !__MASS_H__
