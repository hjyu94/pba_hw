#include "AABB.h"

AABBUPtr AABB::Create(const glm::vec3 start_position, const glm::vec3 end_position)
{
	return AABBUPtr(new AABB(start_position, end_position));
}

AABB::~AABB()
{
}
