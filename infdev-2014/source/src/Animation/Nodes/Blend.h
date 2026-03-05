#pragma once

#include "INode.h"
#include "../MixedFrame.h"

namespace Esteem
{
	namespace Animation
	{
		class Blend : INode
		{
		private:
			INode* from;
			INode* to;

		public:
			virtual void Interpolate(hash_t boneHash, MixedFrame& frame) const override;
			virtual void Update(float time, INode** sequencePtr) override;
		};
	}
}
