#pragma once

namespace Esteem
{
	namespace Partitioning
	{
		struct SpatialObject;

		class ISpatialObjectContainer
		{
		friend struct SpatialObject;

		public:
			virtual void Translate(SpatialObject* cullingObject) = 0;
			virtual void Scale(SpatialObject* cullingObject) = 0;

		private:
			virtual void Remove(SpatialObject* cullingObject) = 0;
		};
	}
}