#pragma once

#include "./Animator.h"

namespace Esteem
{

	inline Animator::Animator(const cgc::strong_ptr<Entity>& entity)
		: AbstractConstituent(entity)
		, enabled(true)
		, lookMode(LookMode::NECK)
		, boneData(nullptr)
		, boneUpperEnd()
	{ }


	inline bool Animator::GetEnabled() const
	{
		return enabled;
	}

	inline void Animator::SetEnabled(bool enable)
	{
		enabled = enable;
	}

	inline void Animator::SetLookAtPosition(const glm::vec3& position)
	{
		lookAtPosition = position;
	}

	inline const glm::vec3& Animator::GetLookAtPosition() const
	{
		return lookAtPosition;
	}

	inline void Animator::SetLookAtEntity(const cgc::weak_ptr<const Entity>& entity)
	{
		lookAtEntity = entity;
	}

	inline const cgc::weak_ptr<const Entity>& Animator::GetLookAtEntity() const
	{
		return lookAtEntity;
	}

	inline Animation::MixedFrame Animator::InterpolateSequences(const Model::BoneData& bone, const ISequence* sequence1, const ISequence* sequence2, float weight)
	{
		Animation::MixedFrame frame(bone);

		// animation 1
		sequence1->Interpolate(bone.hash, frame);

		// animation 2
		if (sequence2 && weight > 0.f)
			sequence2->Interpolate(bone.hash, frame);

		return frame;
	}
}