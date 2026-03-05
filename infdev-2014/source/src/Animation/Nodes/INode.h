#pragma once

namespace Esteem
{
	namespace Animation
	{
		class INode
		{
			virtual ~INode() = 0;

			virtual void Interpolate(hash_t boneHash, MixedFrame& frame) const = 0;
			virtual void Update(float time, INode** sequencePtr) = 0;
		};
	}
}
