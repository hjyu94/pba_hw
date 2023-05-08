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

    friend std::ostream& operator << (std::ostream& out, const AABB& aabb)
    {
        std::cout << "AABB: " << aabb.m_start_position.x << ", " << aabb.m_start_position.y << ", " << aabb.m_start_position.z << " / "
            << aabb.m_end_position.x << ", " << aabb.m_end_position.y << ", " << aabb.m_end_position.z;
        return out; // for chaining
    }

private:
    AABB(glm::vec3 start_position, glm::vec3 end_position)
        : m_start_position(start_position), m_end_position(end_position) {}
};

#endif // __AABB_H__