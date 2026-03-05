#pragma once

#include "stdafx.h"
#include <vector>

namespace mm
{
	class array_view
	{
	private:
		const void* _data;
		size_t _size;
		size_t _type_size;

	public:
		array_view()
			: _data(nullptr)
			, _size(0)
			, _type_size(0)
		{}

		array_view(const void* data, size_t size, size_t type_size)
			: _data(data)
			, _size(size)
			, _type_size(type_size)
		{ }

		template<class T>
		inline array_view(const std::vector<T>& vector)
			: array_view(vector.data(), vector.size(), sizeof(T))
		{ }

		inline const void* data() const { return _data; }
		inline size_t size() const { return _size; }
		inline size_t type_size() const { return _type_size; }

		inline bool empty() const { return _size == 0; }
	};
}