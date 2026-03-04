#include "./ModelFactory.h"

namespace Esteem
{
	inline void ModelFactory::EnlargeBounds(Model& model, const glm::vec3& vertex)
	{
		if (vertex.x > model.volume.x)
			model.volume.x = vertex.x;
		else if (vertex.x < model.center.x)
			model.center.x = vertex.x;

		if (vertex.y > model.volume.y)
			model.volume.y = vertex.y;
		else if (vertex.y < model.center.y)
			model.center.y = vertex.y;

		if (vertex.z > model.volume.z)
			model.volume.z = vertex.z;
		else if (vertex.z < model.center.z)
			model.center.z = vertex.z;
	}
}