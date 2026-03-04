#pragma once

#include <random>

namespace Esteem
{
	class Random
	{
	private:
		static std::mt19937 engine;

	public:
		template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
		static T Value();

		template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
		static T Between(T a, T b);
	};
}

#include "./Random.inl"