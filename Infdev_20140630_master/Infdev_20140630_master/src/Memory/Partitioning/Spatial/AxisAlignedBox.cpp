#include "AxisAlignedBox.h"
#include <SFML/OpenGL.hpp>

AxisAlignedBox::AxisAlignedBox(const glm::vec3& begin, const glm::vec3& end)
	: begin(begin),
	end(end)
{

};

AxisAlignedBox::AxisAlignedBox(const glm::vec3& point, float BoxSize, bool gridBox)
{
	// use the point to determine where on the grid this box should be made
	if (gridBox)
	{
		begin = glm::vec3(floorf(point.x / BoxSize), floorf(point.y / BoxSize), floorf(point.z / BoxSize)) * BoxSize;
		end = glm::vec3(begin.x + BoxSize, begin.y + BoxSize, begin.z + BoxSize);
	}
	// use the point as a center point
	else
	{
		float halfBoxSize = BoxSize * 0.5f;
		begin = glm::vec3(point.x - halfBoxSize, point.y - halfBoxSize, point.z - halfBoxSize);
		end = glm::vec3(point.x + halfBoxSize, point.y + halfBoxSize, point.z + halfBoxSize);
	}
};

AxisAlignedBox::AxisAlignedBox(const glm::ivec3& point, float BoxSize, bool gridBox)
{
	// use the point to determine where on the grid this box should be made
	if(gridBox)
	{
		begin = glm::vec3(floorf(point.x / BoxSize), floorf(point.y / BoxSize), floorf(point.z / BoxSize)) * BoxSize;
		end = glm::vec3(begin.x + BoxSize, begin.y + BoxSize, begin.z + BoxSize);
	}
	// use the point as a center point
	else
	{
		float halfBoxSize = BoxSize * 0.5f;
		begin = glm::vec3(point.x - halfBoxSize, point.y - halfBoxSize, point.z - halfBoxSize);
		end = glm::vec3(point.x + halfBoxSize, point.y + halfBoxSize, point.z + halfBoxSize);
	}
};

bool AxisAlignedBox::IsPointInBox(const glm::vec3& point)
{
	return (point.x >= begin.x && point.x <= end.x &&
		point.y >= begin.y && point.y <= end.y &&
		point.z >= begin.z && point.z <= end.z);
};