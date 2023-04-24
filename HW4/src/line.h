#ifndef __LINE_H__
#define __LINE_H__

#include "common.h"
#include "program.h"
#include <glm/glm.hpp>

CLASS_PTR(Line);
class Line {
public:
    static LinePtr Create(const glm::vec3 start, const glm::vec3 end);
    void Render(const Program* program);
    bool Init(const glm::vec3 start, const glm::vec3 end);
    ~Line();

private:
    Line() {}
    
    glm::vec3 m_startPoint;
    glm::vec3 m_endPoint;
    
    glm::vec3 m_lineColor{ glm::vec3(1.f, 0.f, 1.f) };
};

#endif // __LINE_H__