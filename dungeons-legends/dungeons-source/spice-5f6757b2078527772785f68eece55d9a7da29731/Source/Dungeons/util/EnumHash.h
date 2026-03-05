/*********************************************************
* (c) Mojang.All rights reserved                         *
* (c) Microsoft.All rights reserved.                     *
*********************************************************/
#pragma once

#define ENUM_HASH(enumName) \
namespace std \
{ \
	template<> struct hash<enumName> { \
		using enumType = underlying_type<enumName>::type; \
		size_t operator()(enumName const& enumVal) const { \
			return hash<enumType>()((enumType)enumVal); \
		} \
	}; \
}
