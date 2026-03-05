#pragma once

#include <type_traits>

namespace Esteem
{
	template<class T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
	class Rect
	{
	public:
		T left;   ///< Left coordinate of the rectangle
		T top;    ///< Top coordinate of the rectangle
		T width;  ///< Width of the rectangle
		T height; ///< Height of the rectangle

		Rect(T left, T top, T width, T height)
			: left(left)
			, top(top)
			, width(width)
			, height(height)
		{ }
	};
}