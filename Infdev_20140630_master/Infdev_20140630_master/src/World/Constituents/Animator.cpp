#include "Animator.h"
#include "World/Objects/Entity.h"

#include <glm/glm.hpp>
#include <General/Matrix.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_interpolation.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "Math/Math.h"
#include "Math/Interpolation.h"
#include "Window/View.h"
#include "Utils/Time.h"
#include "Utils/Data.h"
#include "Utils/Debug.h"

#include "World/World.h"
#include "Physics/Physics.h"
#include "Physics/RayCast.h"


namespace Esteem
{
	cgc::strong_ptr<Animator> Animator::Instantiate(const cgc::strong_ptr<Entity>& entity)
	{
		cgc::strong_ptr<Animator> animator = entity->GetWorld()->GetWorldConstituents().animators.Create(entity);
		animator->Initialize();

		return animator;
	}

#define entity GetEntity()

	void Animator::Initialize()
	{
		if (meshRenderer = GetEntity()->GetSystemComponent<MeshRenderer>())
		{
			// create copy
			if (const cgc::strong_ptr<const Model>& model = meshRenderer->GetModel())
			{
				boneMatrices = Data::GetRenderingFactory()->LoadBoneMatrices(model->boneMatrices.size());
				//boneMatrices->UpdateMatrices(model->GetBoneMatrices());

				this->model = model;
				this->boneData = &model->boneData;
				this->boneUpperEnd = model->boneUpperEnd;

				// get animations from model
				animationCollection = model->GetBoneAnimationCollection();
				if (animationCollection != nullptr && animationCollection->GetAnimationSequences().size() > 0)
				{
					if (sequences.empty())
						sequences.resize(1);

					sequences[0] = new Sequence(animationCollection->GetAnimationSequences().begin()->second, 0.f);
				}

				// apply bonematrices to all renderobjects
				if (CullingObject* cullingObject = meshRenderer->GetCullingObject())
				{
					for (auto& renderObject : cullingObject->GetRenderObjects())
						renderObject->boneMatrices = boneMatrices;
				}
			}
		}
	}

	void Animator::LoadAnimation(size_t animationIndex, float weight, const std::string& path)
	{
		ModelFactory& modelFactory = Data::GetModelFactory();
		std::string extensionLess = path.substr(0, path.find_last_of('.'));
		cgc::strong_ptr<const Model> model = modelFactory.LoadModel(path, extensionLess, Model::ModelGenerateSettings::GENERATE_NORMALS);

		animationCollection = model->GetBoneAnimationCollection();
		if (animationCollection != nullptr && animationCollection->GetAnimationSequences().size() > 0)
		{
			if (animationIndex >= sequences.size())
				sequences.resize(animationIndex + 1);

			sequences[animationIndex] = new Sequence(animationCollection->GetAnimationSequences().begin()->second, 0.f);
		}
	}

	void Animator::Update()
	{
		for (auto* seq : sequences)
			seq->Update(Time::RenderDeltaTime(), &seq);

		// root
		auto bones = boneData->cbegin();
		auto bonesEnd = boneData->cend();
		const Model::BoneData& root = bones[0];
		
		std::vector<Animation::MixedFrame> localTransforms;
		localTransforms.reserve(boneData->size());
		localTransforms.push_back(InterpolateSequences(root, sequences[0], nullptr, 0.f));
		localTransforms[0].t.x = 0.f;
		localTransforms[0].t.z = 0.f;

		if (root.childCount)
		{
			// spine and up
			while (++bones != boneUpperEnd)
				localTransforms.push_back(InterpolateSequences(*bones, sequences[1], nullptr, 0.f));

			// legs and/or others
			while (bones != bonesEnd)
				localTransforms.push_back(InterpolateSequences(*bones++, sequences[0], nullptr, 0.f));
		}

		auto bonesPtr = boneData->data();

		auto* bMatrices = this->boneMatrices->GetMatrices();
		auto* lTransforms = localTransforms.data();
		RecursiveChildUpdateMatrices(bonesPtr, lTransforms, bMatrices, Esteem::BoneMatrices::value_type(1));

		/*Animation::MixedFrame frame = InterpolateSequences(root, sequences[0], nullptr, 0.f);
		glm::mat3x4 matrix = glm::gtx::to_row_major_mat<3>(frame.r, frame.s, glm::vec3(0, frame.t.y, 0));
		boneMatrices->SetMatrix(0, glm::gtx::xmul(root.invBindMatrix, matrix));
		
		if (size_t childCount = root.childCount)
		{
			// spine and up
			RecursiveAnimationPlay(++bones, sequences[1], nullptr, 1.f, matrix);

			// legs and/or others
			while (--childCount > 0)
				RecursiveAnimationPlay(++bones, sequences[0], nullptr, 0.f, matrix);
		}*/

		boneMatrices->UpdateMatrices();
	}

	void Animator::RecursiveAnimationPlay(const Model::BoneData* bone, const Model::BoneData* end, std::vector<Animation::MixedFrame>& transform, const ISequence* sequence0, const ISequence* sequence1, float weight)
	{
		while (bone < end)
			transform.push_back(InterpolateSequences(*bone++, sequence0, sequence1, weight));
	}

	void Animator::RecursiveChildUpdateMatrices(const Model::BoneData*& bone, Animation::MixedFrame*& transform, Esteem::BoneMatrices::value_type*& matrices, const Esteem::BoneMatrices::value_type& parentMatrix)
	{
		if (!bone->physicsIK)
		{
			glm::mat3x4 matrix = glm::gtx::xmul(glm::gtx::to_row_major_mat<3>(transform->r, transform->s, transform->t), parentMatrix);
			*matrices = glm::gtx::xmul(bone->invBindMatrix, matrix);

			for (size_t count = bone->childCount; count-- > 0;)
				RecursiveChildUpdateMatrices(++bone, ++transform, ++matrices, matrix);
		}
		else
			RayCastIK(bone, transform, matrices, parentMatrix);
	}

	void Animator::RayCastIK(const Model::BoneData*& bone, Animation::MixedFrame*& transform, Esteem::BoneMatrices::value_type*& matrices, const Esteem::BoneMatrices::value_type& parentMatrix)
	{
		const Animation::MixedFrame& frame = transform[0];
		glm::mat3x4 matrix = glm::gtx::xmul(glm::gtx::to_row_major_mat<3>(transform->r, transform->s, transform->t), parentMatrix);

		const Model::BoneData& lowerLegBone = bone[1];
		const Animation::MixedFrame& lowerLegFrame = transform[1];
		glm::mat3x4 lowerLegMatrix = glm::gtx::xmul(glm::gtx::to_row_major_mat<3>(lowerLegFrame.r, lowerLegFrame.s, lowerLegFrame.t), matrix);

		const Model::BoneData& footBone = bone[2];
		const Animation::MixedFrame& footFrame = transform[2];
		glm::mat3x4 footMatrix = glm::gtx::xmul(glm::gtx::to_row_major_mat<3>(footFrame.r, footFrame.s, footFrame.t), lowerLegMatrix);

		// find new foot position
		const glm::vec3 upperLegPosition(matrix[0][3], matrix[1][3], matrix[2][3]);
		const glm::vec3 footPosition(footMatrix[0][3], footMatrix[1][3], footMatrix[2][3]);
		glm::vec3 up = upperLegPosition - footPosition;
		//float upLength = glm::length2(up);

		const glm::mat4& m = entity->GetMatrix();
		const glm::mat4 invM = glm::inverse(m);

		Physics& physics = entity->GetWorld()->Physics();
		RayCastInfo rayInfo(glm::vec3(m * glm::vec4(upperLegPosition, 1.f)), glm::vec3(m * glm::vec4(-up, 1.f)));
		physics.RayCast(rayInfo.origin, glm::vec3(m * glm::vec4(footPosition, 1.f)), rayInfo);

		if (rayInfo.hasHit)
		{
			up = glm::normalize(up);
			rayInfo.hitPoint = glm::vec3(invM * glm::vec4(rayInfo.hitPoint, 1.f));
			rayInfo.hitNormal = -glm::normalize(glm::vec3(invM * glm::vec4(rayInfo.hitNormal, 1.f)));

			const float footHeight = 11.f;

			const glm::vec3 lowerLegPosition(lowerLegMatrix[0][3], lowerLegMatrix[1][3], lowerLegMatrix[2][3]);
			const glm::vec3 newFootPosition = rayInfo.hitPoint
				+ up * (1.f + tan((1.f - glm::dot(up, rayInfo.hitNormal)) * float(M_HALF_PI)) * float(M_HALF_PI)) * footHeight;

			glm::mat3 rotMatrix;
			rotMatrix[1] = glm::normalize(upperLegPosition - newFootPosition);
			rotMatrix[0] = glm::normalize(glm::cross(rotMatrix[1], lowerLegPosition - newFootPosition));
			rotMatrix[2] = glm::cross(rotMatrix[0], rotMatrix[1]);

			const glm::quat baseRotation(rotMatrix);

			__m128 distance2;
			float(&d2)[4] = distance2.m128_f32;
			d2[0] = glm::distance2(upperLegPosition, lowerLegPosition);
			d2[1] = glm::distance2(lowerLegPosition, footPosition);
			d2[2] = glm::distance2(upperLegPosition, newFootPosition);
			d2[3] = 0.f;

			// rsqrt(x)
			__m128 rqsrt = _mm_rsqrt_ps(distance2);

			// 1 NR iteration
			__m128 three = _mm_set1_ps(3.0f), half = _mm_set1_ps(0.5f);
			__m128 muls = _mm_mul_ps(_mm_mul_ps(distance2, rqsrt), rqsrt);
			rqsrt = _mm_mul_ps(_mm_mul_ps(half, rqsrt), _mm_sub_ps(three, muls));

			// x * rsqrt(x)
			__m128 distance = _mm_mul_ps(distance2, rqsrt);
			float(&d)[4] = distance.m128_f32;

			if (d[2] >= d[0] + d[1])
			{
				matrix = glm::gtx::to_row_major_mat<3>(baseRotation, frame.s, upperLegPosition);
				lowerLegMatrix = glm::gtx::xmul(glm::gtx::to_row_major_mat<3>(glm::quat(), lowerLegFrame.s, lowerLegFrame.t), matrix);
			}
			else
			{
				//const float aAngle = acos((d2[1] + d2[2] - d2[0]) / (2.f * d[1] * d[2]));
				const float bAngle = acos((d2[0] + d2[2] - d2[1]) / (2.f * d[0] * d[2]));
				const float cAngle = acos((d2[0] + d2[1] - d2[2]) / (2.f * d[0] * d[1]));

				matrix = glm::gtx::to_row_major_mat<3>(glm::quat(glm::rotate(baseRotation, -bAngle, glm::vec3(1, 0, 0))), frame.s, upperLegPosition);
				lowerLegMatrix = glm::gtx::xmul(glm::gtx::to_row_major_mat<3>(glm::quat(glm::rotate(float(M_PI) - cAngle, glm::vec3(1, 0, 0))), lowerLegFrame.s, lowerLegFrame.t), matrix);
			}

			glm::vec3 forward(footMatrix[0][2], footMatrix[1][2], footMatrix[2][2]);
			glm::vec3 right = glm::normalize(glm::cross(rayInfo.hitNormal, forward));
			forward = glm::cross(right, rayInfo.hitNormal);

			footMatrix[0][0] = right[0];
			footMatrix[1][0] = right[1];
			footMatrix[2][0] = right[2];

			footMatrix[0][1] = rayInfo.hitNormal[0];
			footMatrix[1][1] = rayInfo.hitNormal[1];
			footMatrix[2][1] = rayInfo.hitNormal[2];

			footMatrix[0][2] = forward[0];
			footMatrix[1][2] = forward[1];
			footMatrix[2][2] = forward[2];

			footMatrix[0][3] = newFootPosition[0];
			footMatrix[1][3] = newFootPosition[1];
			footMatrix[2][3] = newFootPosition[2];
		}

		matrices[0] = glm::gtx::xmul(bone->invBindMatrix, matrix);
		matrices[1] = glm::gtx::xmul(lowerLegBone.invBindMatrix, lowerLegMatrix);
		matrices[2] = glm::gtx::xmul(footBone.invBindMatrix, footMatrix);

		bone += 2;
		transform += 2;
		matrices += 2;

		for (size_t count = footBone.childCount; count-- > 0;)
			RecursiveChildUpdateMatrices(++bone, ++transform, ++matrices, matrix);
	}

	void Animator::RecursiveAnimationPlay(const Model::BoneData*& bones, const ISequence* sequence1, const ISequence* sequence2, float weight, const glm::mat3x4& parentMatrix)
	{
		const Model::BoneData& bone = bones[0];

		if (true) //(bone.hash != CT_HASH("Head"))
		{
			Animation::MixedFrame frame = InterpolateSequences(bone, sequence1, sequence2, weight);
			glm::mat3x4 matrix = glm::gtx::xmul(glm::gtx::to_row_major_mat<3>(frame.r, frame.s, frame.t), parentMatrix);

			if (bone.hash == CT_HASH("LeftUpLeg") || bone.hash == CT_HASH("RightUpLeg"))
			{
				const Model::BoneData& lowerLegBone = bones[1];
				Animation::MixedFrame lowerLegFrame = InterpolateSequences(lowerLegBone, sequence1, sequence2, weight);
				glm::mat3x4 lowerLegMatrix = glm::gtx::xmul(glm::gtx::to_row_major_mat<3>(lowerLegFrame.r, lowerLegFrame.s, lowerLegFrame.t), matrix);
				
				const Model::BoneData& footBone = bones[2];
				Animation::MixedFrame footFrame = InterpolateSequences(footBone, sequence1, sequence2, weight);
				glm::mat3x4 footMatrix = glm::gtx::xmul(glm::gtx::to_row_major_mat<3>(footFrame.r, footFrame.s, footFrame.t), lowerLegMatrix);

				// find new foot position
				const glm::vec3 upperLegPosition(matrix[0][3], matrix[1][3], matrix[2][3]);
				const glm::vec3 footPosition(footMatrix[0][3], footMatrix[1][3], footMatrix[2][3]);
				glm::vec3 up = upperLegPosition - footPosition;
				//float upLength = glm::length2(up);

				const glm::mat4& m = entity->GetMatrix();
				const glm::mat4 invM = glm::inverse(m);

				Physics& physics = entity->GetWorld()->Physics();
				RayCastInfo rayInfo(glm::vec3(m * glm::vec4(upperLegPosition, 1.f)), glm::vec3(m * glm::vec4(-up, 1.f)));
				physics.RayCast(rayInfo.origin, glm::vec3(m * glm::vec4(footPosition, 1.f)), rayInfo);

				if (rayInfo.hasHit)
				{
					up = glm::normalize(up);
					rayInfo.hitPoint = glm::vec3(invM * glm::vec4(rayInfo.hitPoint, 1.f));
					rayInfo.hitNormal = -glm::normalize(glm::vec3(invM * glm::vec4(rayInfo.hitNormal, 1.f)));

					const float footHeight = 11.f;

					const glm::vec3 lowerLegPosition(lowerLegMatrix[0][3], lowerLegMatrix[1][3], lowerLegMatrix[2][3]);
					const glm::vec3 newFootPosition = rayInfo.hitPoint
						+ up * (1.f + tan((1.f - glm::dot(up, rayInfo.hitNormal)) * float(M_HALF_PI)) * float(M_HALF_PI)) * footHeight;

					glm::mat3 rotMatrix;
					rotMatrix[1] = glm::normalize(upperLegPosition - newFootPosition);
					rotMatrix[0] = glm::normalize(glm::cross(rotMatrix[1], lowerLegPosition - newFootPosition));
					rotMatrix[2] = glm::cross(rotMatrix[0], rotMatrix[1]);

					const glm::quat baseRotation(rotMatrix);

					__m128 distance2;
					float(&d2)[4] = distance2.m128_f32;
					d2[0] = glm::distance2(upperLegPosition, lowerLegPosition);
					d2[1] = glm::distance2(lowerLegPosition, footPosition);
					d2[2] = glm::distance2(upperLegPosition, newFootPosition);
					d2[3] = 0.f;

					// rsqrt(x)
					__m128 rqsrt = _mm_rsqrt_ps(distance2);

					// 1 NR iteration
					__m128 three = _mm_set1_ps(3.0f), half = _mm_set1_ps(0.5f);
					__m128 muls = _mm_mul_ps(_mm_mul_ps(distance2, rqsrt), rqsrt);
					rqsrt = _mm_mul_ps(_mm_mul_ps(half, rqsrt), _mm_sub_ps(three, muls));

					// x * rsqrt(x)
					__m128 distance = _mm_mul_ps(distance2, rqsrt);
					float(&d)[4] = distance.m128_f32;

					if (d[2] >= d[0] + d[1])
					{
						matrix = glm::gtx::to_row_major_mat<3>(baseRotation, frame.s, upperLegPosition);
						lowerLegMatrix = glm::gtx::xmul(glm::gtx::to_row_major_mat<3>(glm::quat(), lowerLegFrame.s, lowerLegFrame.t), matrix);
					}
					else
					{
						//const float aAngle = acos((d2[1] + d2[2] - d2[0]) / (2.f * d[1] * d[2]));
						const float bAngle = acos((d2[0] + d2[2] - d2[1]) / (2.f * d[0] * d[2]));
						const float cAngle = acos((d2[0] + d2[1] - d2[2]) / (2.f * d[0] * d[1]));

						matrix = glm::gtx::to_row_major_mat<3>(glm::quat(glm::rotate(baseRotation, -bAngle, glm::vec3(1, 0, 0))), frame.s, upperLegPosition);
						lowerLegMatrix = glm::gtx::xmul(glm::gtx::to_row_major_mat<3>(glm::quat(glm::rotate(float(M_PI) - cAngle, glm::vec3(1, 0, 0))), lowerLegFrame.s, lowerLegFrame.t), matrix);
					}

					glm::vec3 forward(footMatrix[0][2], footMatrix[1][2], footMatrix[2][2]);
					glm::vec3 right = glm::normalize(glm::cross(rayInfo.hitNormal, forward));
					forward = glm::cross(right, rayInfo.hitNormal);

					footMatrix[0][0] = right[0];
					footMatrix[1][0] = right[1];
					footMatrix[2][0] = right[2];

					footMatrix[0][1] = rayInfo.hitNormal[0];
					footMatrix[1][1] = rayInfo.hitNormal[1];
					footMatrix[2][1] = rayInfo.hitNormal[2];

					footMatrix[0][2] = forward[0];
					footMatrix[1][2] = forward[1];
					footMatrix[2][2] = forward[2];

					footMatrix[0][3] = newFootPosition[0];
					footMatrix[1][3] = newFootPosition[1];
					footMatrix[2][3] = newFootPosition[2];
				}

				boneMatrices->SetMatrix(bone.index, glm::gtx::xmul(bone.invBindMatrix, matrix));
				boneMatrices->SetMatrix(lowerLegBone.index, glm::gtx::xmul(lowerLegBone.invBindMatrix, lowerLegMatrix));
				boneMatrices->SetMatrix(footBone.index, glm::gtx::xmul(footBone.invBindMatrix, footMatrix));

				bones += 2;

				for (size_t count = bone.childCount; count-- > 0;)
					RecursiveAnimationPlay(++bones, sequence1, sequence2, weight, footMatrix);
			}
			else
			{
				boneMatrices->SetMatrix(bone.index, glm::gtx::xmul(bone.invBindMatrix, matrix));

				for (size_t count = bone.childCount; count-- > 0;)
					RecursiveAnimationPlay(++bones, sequence1, sequence2, weight, matrix);
			}
		}
		else // head
		{
			Animation::MixedFrame frame({} /*bone.defaults.rotation*/, bone.defaults.scale, bone.defaults.translation);

			glm::vec3 lookPos = lookAtEntity
				? static_cast<glm::vec3>(lookAtEntity->GetPosition() + lookAtEntity->GetUp() * -0.1f)
				: lookAtPosition; /*+ lookAtEntity->GetUp() * 1.8f*/

			for (size_t i = 0; i < 3; ++i)
				frame.t[i] = frame.t[i] * parentMatrix[i][0] + frame.t[i] * parentMatrix[i][1] + frame.t[i] * parentMatrix[i][2] + parentMatrix[i][3];

			glm::quat parentRotation(static_cast<glm::mat3>(parentMatrix));
			glm::vec3 direction = glm::normalize(lookPos - (entity->GetPosition() + frame.t * entity->GetScale())) * glm::inverse(parentRotation);

			glm::vec2 headEuler(
				glm::clamp(std::atan2(direction.x, direction.z), -glm::pi<float>() * 0.45f, glm::pi<float>() * 0.45f),
				glm::clamp(std::asin(direction.y), -glm::pi<float>() * 0.25f, glm::pi<float>() * 0.35f));

			direction = glm::vec3(std::sin(headEuler.x), std::sin(headEuler.y), std::cos(headEuler.x)) * parentRotation;

			glm::quat targetR(glm::vec3(-std::asin(direction.y), std::atan2(direction.x, direction.z), 0.f));
			frame.r = glm::slerp(lastHeadRotation, targetR, std::clamp(Time::RenderDeltaTime() * 10.f, 0.f, 1.f));
			lastHeadRotation = frame.r;

			boneMatrices->SetMatrix(bone.index, glm::gtx::to_row_major_mat<3>(frame.r, frame.s, frame.t));

			for (size_t count = bone.childCount; count-- > 0;)
				RecursiveAnimationPlay(++bones, sequence1, sequence2, weight, boneMatrices->GetMatrix(bone.index));
		}
	}

	Animation::MixedFrame Animator::InterpolateFrame(const AnimationChannelData& channel, float time)
	{
		Animation::MixedFrame frame(glm::quat(time < channel.rotationKeys.front().time ? channel.rotationKeys.front().value : channel.rotationKeys.back().value)
			, glm::vec3(1.f)//  (channel.scaleKeys.empty() ? channel.scaleKeys.front().value : channel.scaleKeys.back().value);
			, glm::vec3(time < channel.positionKeys.front().time ? channel.positionKeys.front().value : channel.positionKeys.back().value));

		// interpolate current rotation
		for (uint key = 1; key < channel.rotationKeys.size(); ++key)
		{
			const AnimationKey<glm::quat>& rotationKey = channel.rotationKeys[key];
			if (rotationKey.time > time)
			{
				const AnimationKey<glm::quat>& prevRotationKey = channel.rotationKeys[key - 1];

				float deltaCurrentTime = time - prevRotationKey.time;
				float deltaAnimationTime = rotationKey.time - prevRotationKey.time;
				float factor = glm::clamp(deltaCurrentTime / deltaAnimationTime, 0.f, 1.f);

				frame.r = glm::slerp(prevRotationKey.value, rotationKey.value, factor);
				break;
			}
		}

		// interpolate current scaling
		/*for (uint key = 1; key < channelData.scaleKeys.size(); ++key)
		{
			if (channelData.scaleKeys[key].time > time)
			{
				float deltaCurrentTime = time - channelData.scaleKeys[key - 1].time;
				float deltaAnimationTime = channelData.scaleKeys[key].time - channelData.scaleKeys[key - 1].time;
				float factor = deltaCurrentTime / deltaAnimationTime;

				frame.s = glm::mix(channelData.scaleKeys[key - 1].value, channelData.scaleKeys[key].value, factor);
				break;
			}
		}*/

		//if (boneIndex == 0)
		{
			// interpolate current position
			for (uint key = 1; key < channel.positionKeys.size(); ++key)
			{
				const AnimationKey<glm::vec3>& positionKey = channel.positionKeys[key];
				if (positionKey.time > time)
				{
					const AnimationKey<glm::vec3>& prevPositionKey = channel.positionKeys[key - 1];

					float deltaCurrentTime = time - prevPositionKey.time;
					float deltaAnimationTime = positionKey.time - prevPositionKey.time;
					float factor = deltaCurrentTime / deltaAnimationTime;

					frame.t = glm::mix(prevPositionKey.value, positionKey.value, factor);
					break;
				}
			}
		}

		return frame;
	}

	void Animator::HeadTargetIK(const glm::vec3& relativePosition)
	{
		auto found = model->boneMap.find(CT_HASH("Head"));
		if (found != model->boneMap.end())
		{
			const Model::BoneData& bone = boneData->at(found->second);
			const Model::BoneData& parentBone = boneData->at(bone.parentIndex);
			glm::mat3x4 parentMatrix = glm::gtx::xmul(boneMatrices->GetMatrix(bone.parentIndex), glm::inverse(glm::mat4(parentBone.invBindMatrix)));

			glm::quat rotation = glm::lookAt(glm::vec3(0.f), relativePosition, Vector3::up);
			glm::mat3x4 localMatrix = glm::gtx::to_row_major_mat<3>(rotation, glm::vec3(1.f), glm::vec3(0.f));
			glm::mat3x4 m = glm::gtx::xmul(localMatrix, parentMatrix);
			boneMatrices->SetMatrix(bone.index, glm::gtx::xmul(bone.invBindMatrix, m));
		}
	}

	void Animator::Sequence::Interpolate(hash_t boneHash, Animation::MixedFrame& frame) const
	{
		auto& channels = sequence->GetChannelData();
		auto channelFound = channels.find(boneHash);
		if (channelFound != channels.end())
			frame = Animator::InterpolateFrame(channelFound->second, time);
	}

	void Animator::ArrayMixer::Interpolate(hash_t boneHash, Animation::MixedFrame& frame) const
	{
		const auto& sequence1 = *from;
		const auto& sequence2 = *to;
		const auto& channels1 = sequence1.sequence->GetChannelData();
		const auto& channels2 = sequence2.sequence->GetChannelData();

		auto channelFound1 = channels1.find(boneHash);
		if (channelFound1 != channels1.end())
			frame = Animator::InterpolateFrame(channelFound1->second, sequence1.time);

		auto channelFound2 = channels2.find(boneHash);
		if (channelFound2 != channels2.end())
		{
			Animation::MixedFrame frame2 = Animator::InterpolateFrame(channelFound2->second, sequence2.time);

			frame.t += (frame2.t - frame.t) * delta;
			frame.s += (frame2.s - frame.s) * delta;
			frame.r = glm::slerp(frame.r, frame2.r, delta);
		}
	}
}

#undef entity