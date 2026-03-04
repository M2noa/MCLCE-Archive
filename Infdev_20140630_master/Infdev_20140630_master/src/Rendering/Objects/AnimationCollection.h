#pragma once

#include <unordered_map>
#include <cppu/cgc/pointers.h>
#include <cppu/hash.h>

namespace Esteem
{
	class AnimationSequence;

	class AnimationCollection
	{
	private:
		std::string path;
		std::unordered_map<hash_t, cgc::strong_ptr<const AnimationSequence>> sequences;

	public:
		AnimationCollection(std::string_view path, const std::unordered_map<hash_t, cgc::strong_ptr<const AnimationSequence>>& sequences)
			: path(path)
			, sequences(sequences)
		{}

		AnimationCollection(std::string_view path, std::unordered_map<hash_t, cgc::strong_ptr<const AnimationSequence>>&& sequences)
			: path(path)
			, sequences(std::move(sequences))
		{}

		const std::string& GetPath() const { return path; }
		const std::unordered_map<hash_t, cgc::strong_ptr<const AnimationSequence>>& GetAnimationSequences() const { return sequences; }
	};
}