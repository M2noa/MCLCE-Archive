#pragma once

#include "./AbstractConstituent.h"

#include <algorithm>

#include "Rendering/Objects/BoneMatrices.h"
#include "Rendering/Objects/AnimationSequence.h"
#include "Rendering/Objects/AnimationCollection.h"
#include "./MeshRenderer.h"

#include <cppu/cgc/array.h>

#include "Animation/MixedFrame.h"
#include "Animation/Nodes/INode.h"

namespace Esteem
{
	class Animator : public AbstractConstituent
	{
	public:
		enum AnimationFlags
		{
			NONE = 0,
			LOOP = 1,
			BODY_UPPER = 2,
			BODY_LOWER = 4,


			BODY_WHOLE = BODY_UPPER | BODY_LOWER
		};

		struct ISequence
		{
			virtual void Interpolate(hash_t boneHash, Animation::MixedFrame& frame) const = 0;
			virtual void Update(float time, ISequence** sequencePtr) = 0;
		};

		struct Sequence : public ISequence
		{
			cgc::strong_ptr<const AnimationSequence> sequence;
			float time = 0.f;
			float speed = 1.f;

			Sequence(const cgc::strong_ptr<const AnimationSequence>& sequence, float time)
				: sequence(sequence)
				, time(time)
			{ }

			virtual void Interpolate(hash_t boneHash, Animation::MixedFrame& frame) const;
			virtual void Update(float time, ISequence** sequencePtr)
			{
				this->time = std::fmodf(this->time + time * sequence->GetTPS(), sequence->GetDuration());
			}
		};

		struct ArrayMixer : public ISequence
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

			virtual void Interpolate(hash_t boneHash, Animation::MixedFrame& frame) const;
			virtual void Update(float time, ISequence** sequencePtr) { for (auto& sequence : sequences) sequence.Update(time, sequencePtr); }
		};

		/*struct Vector2DMixer : public ISequence
		{
			float rotation;
			float length;

			float deltaRotation;
			float deltaLength;

			std::array<std::array<Sequence, 3>, 8> sequences;

			virtual void Interpolate(hash_t boneHash, MixedFrame& frame) const;
			virtual void Update(float time, ISequence** sequencePtr) { for (auto& sequences2 : sequences) for (auto& sequence : sequences2) sequence.Update(time, sequencePtr); }

		};*/

	private:
		cgc::strong_ptr<BoneMatrices> boneMatrices;
		const std::vector<Model::BoneData>* boneData;
		std::vector<Model::BoneData>::const_iterator boneUpperEnd;

		cgc::strong_ptr<const Model> model;
		cgc::strong_ptr<MeshRenderer> meshRenderer;
		cgc::strong_ptr<const AnimationCollection> animationCollection;

		std::vector<ISequence*> sequences;
		cgc::weak_ptr<const Entity> lookAtEntity;

		Quaternion lastHeadRotation;
		Vector3 lookAtPosition;
		bool enabled;

		enum class LookMode
		{
			NONE = 0,
			EYES = 1,
			HEAD = 2,
			NECK = 4,
			ALL = EYES | HEAD | NECK
		} lookMode;

		void RecursiveAnimationPlay(const Model::BoneData*& bone, const ISequence* sequence1, const ISequence* sequence2, float weight, const glm::mat3x4& parentMatrix);
		void RecursiveChildUpdateMatrices(const Model::BoneData*& bone, Animation::MixedFrame*& transform, Esteem::BoneMatrices::value_type*& matrices, const Esteem::BoneMatrices::value_type& parentMatrix);
		void RayCastIK(const Model::BoneData*& bone, Animation::MixedFrame*& transform, Esteem::BoneMatrices::value_type*& matrices, const Esteem::BoneMatrices::value_type& parentMatrix);

		void RecursiveAnimationPlay(const Model::BoneData* bone, const Model::BoneData* end, std::vector<Animation::MixedFrame>& transform, const ISequence* sequence0, const ISequence* sequence1, float weight);

		static Animation::MixedFrame InterpolateSequences(const Model::BoneData& bone, const ISequence* sequence1, const ISequence* sequence2, float weight);
		static Animation::MixedFrame InterpolateFrame(const AnimationChannelData& channel, float time);

		void HeadTargetIK(const glm::vec3& relativePosition);

	public:
		Animator(const cgc::strong_ptr<Entity>& entity);

		static cgc::strong_ptr<Animator> Instantiate(const cgc::strong_ptr<Entity>& entity);
		void Initialize();

		void Update();

		void LoadAnimation(size_t animationIndex, float weight, const std::string& path);

		void SetIdleAnimation(size_t animationIndex, float speed, float blendInTime, float blendOutTime, AnimationFlags flags = AnimationFlags::BODY_WHOLE, float offset = 0.f);
		void PlayAnimation(size_t animationIndex, float speed, float blendInTime, float blendOutTime, AnimationFlags flags = AnimationFlags::BODY_WHOLE, float offset = 0.f);

		bool GetEnabled() const;
		void SetEnabled(bool enable);

		void SetLookAtPosition(const glm::vec3& position);
		const glm::vec3& GetLookAtPosition() const;

		void SetLookAtEntity(const cgc::weak_ptr<const Entity>& entity);
		const cgc::weak_ptr<const Entity>& GetLookAtEntity() const;
	};
}

#include "./Animator.inl"