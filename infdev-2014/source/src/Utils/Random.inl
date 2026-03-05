#pragma once

#include "./Random.h"

namespace Esteem
{
	template<typename T, typename>
	inline T Random::Value()
	{
		if constexpr (std::is_integral_v<T>)
		{
			std::uniform_int_distribution<T> dis(T(0), T(100));
			return dis(engine);
		}
		else
		{
			std::uniform_real_distribution<T> dis(T(0.0), T(1.0));
			return dis(engine);
		}
	}

	template<typename T, typename>
	inline T Random::Between(T a, T b)
	{
		if constexpr (std::is_integral_v<T>)
		{
			std::uniform_int_distribution<T> dis(a, b);
			return dis(engine);
		}
		else
		{
			std::uniform_real_distribution<T> dis(a, b);
			return dis(engine);
		}
	}
}