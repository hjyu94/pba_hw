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

    friend std::ostream& operator << (std::ostream& out, const Line& l)
    {
        std::cout << "[Line] start point: " << l.m_startPoint.x << ", " << l.m_startPoint.y << ", " << l.m_startPoint.z 
            << " / end point: " << l.m_endPoint.x << ", " << l.m_endPoint.y << ", " << l.m_endPoint.z
            << " / length: " << glm::distance(l.m_startPoint, l.m_endPoint);
        return out; // for chaining
    }

private:
    Line() {}
    
    glm::vec3 m_startPoint;
    glm::vec3 m_endPoint;
    
    unsigned int VBO, VAO;

    glm::vec3 m_lineColor{ glm::vec3(1.f, 0.f, 1.f) };
};

#endif // __LINE_H__