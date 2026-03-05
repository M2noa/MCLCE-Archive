#pragma once

#include "stdafx.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <cppu/hash.h>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Esteem
{
	template<typename T>
	struct AnimationKey
	{
		float time;
		T value;

		AnimationKey(float time, T value) : time(time), value(value) {}
	};

	struct AnimationChannelData
	{
		std::string boneName;
		uint boneIndex;

		//std::vector<AnimationKey<glm::mat4>> keys;
		std::vector<AnimationKey<glm::quat>> rotationKeys;
		//std::vector<AnimationKey<glm::vec3>> scaleKeys;
		std::vector<AnimationKey<glm::vec3>> positionKeys;
	};

	class AnimationSequence
	{
	private:
		std::string name;
		float duration;
		float tps;

		std::unordered_map<hash_t, AnimationChannelData> channelData;

	public:
		AnimationSequence(const std::string& name, float duration, float tps = 30.f)
			: name(name), duration(duration), tps(tps) {}

		const std::unordered_map<hash_t, AnimationChannelData>& GetChannelData() const { return channelData; }
		inline float GetDuration() const { return duration; }
		inline float GetTPS() const { return tps; }
	};
}