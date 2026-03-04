#pragma once

#include "INode.h"

namespace Esteem
{
	namespace Animation
	{
		class Array : INode
		{
		public:
			std::vector<Sequence> sequences;

		private:
			float weight = 0.f;
			float delta = 0.f;
			Sequence* from;
			Sequence* to;

		public:
			ArrayMixer()
				: sequences()
				, from(nullptr)
				, to(nullptr)
			{ }

			inline float GetMaxWeight() { return (float)sequences.size(); }
			inline float GetWeight() { return this->weight; }
			inline void SetWeight(float weight)
			{
				size_t size = sequences.size();
				weight = std::fmodf(weight, (float)size);
				this->weight = weight;

				float floor = std::floor(weight);
				this->delta = weight - floor;

				size_t index = size_t(floor);
				from = sequences.data() + index;
				to = ++index >= size ? sequences.data() : sequences.data() + index;
			}

			virtual void Interpolate(hash_t boneHash, MixedFrame& frame) const;
			virtual void Update(float time, ISequence** sequencePtr) { for (auto& sequence : sequences) sequence.Update(time, sequencePtr); }
		};
	}
}
