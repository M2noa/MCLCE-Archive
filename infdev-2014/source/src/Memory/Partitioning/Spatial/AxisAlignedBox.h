#pragma once

#include <glm/glm.hpp>

/*
AxisAlignedBox
*/
class AxisAlignedBox
{
private:

public:
	glm::vec3 begin;
	glm::vec3 end;

	AxisAlignedBox() = default;
	AxisAlignedBox(const glm::vec3& begin, const glm::vec3& end);
	AxisAlignedBox(const glm::vec3& point, float BoxSize, bool gridBox = true);
	AxisAlignedBox(const glm::ivec3& point, float BoxSize, bool gridBox = true);

	bool IsPointInBox(const glm::vec3& point);
};