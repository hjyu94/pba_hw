#ifndef __AABB_H__
#define __AABB_H__

#include "common.h"

CLASS_PTR(AABB)
class AABB {
public:
    static AABBUPtr Create(const glm::vec3 start_position, const glm::vec3 end_position);
    ~AABB();

    glm::vec3 m_start_position;
    glm::vec3 m_end_position;

private:
    AABB(glm::vec3 start_position, glm::vec3 end_position)
        : m_start_position(start_position), m_end_position(end_position) {}
};

#endif // __AABB_H__