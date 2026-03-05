#pragma once

// EnumCast is a compile time function that automatically deduces the correct type to static cast an enum to

// this is just a stub class, should never be used
template<typename, typename = void>
struct EnumCastHelper;

template<typename EnumType>
struct EnumCastHelper<EnumType, typename std::enable_if<std::is_enum<EnumType>::value>::type> {
	typedef typename std::underlying_type<EnumType>::type type;
};

// just pass through if integral
template<typename NonEnumType>
struct EnumCastHelper<NonEnumType, typename std::enable_if<std::is_integral<NonEnumType>::value>::type> {
	typedef NonEnumType type;
};

#ifdef MCPE_COMPILER_MSVC
#define enum_cast( EnumValue ) static_cast<EnumCastHelper<typename std::remove_reference<decltype(EnumValue)>::type>::type>(EnumValue)
#else
#define enum_cast( EnumValue ) static_cast<typename EnumCastHelper<typename std::remove_reference<decltype(EnumValue)>::type>::type>(EnumValue)
#endif

#define enum_bitwise_operators(EnumValue,EnumType) \
	inline EnumValue operator | (EnumValue a, EnumValue b) { return EnumValue(((unsigned int)a) | ((unsigned int)b)); } \
	inline EnumValue &operator |= (EnumValue &a, EnumValue b) { return (EnumValue &)(((unsigned int &)a) |= ((unsigned int)b)); } \
	inline EnumValue operator & (EnumValue a, EnumValue b) { return EnumValue(((unsigned int)a) & ((unsigned int)b)); } \
	inline EnumValue &operator &= (EnumValue &a, EnumValue b) { return (EnumValue &)(((unsigned int &)a) &= ((unsigned int)b)); } \
	inline EnumValue operator ~ (EnumValue a) { return EnumValue(~((unsigned int)a)); } \
	inline EnumValue operator ^ (EnumValue a, EnumValue b) { return EnumValue(((unsigned int)a) ^ ((unsigned int)b)); } \
	inline EnumValue &operator ^= (EnumValue &a, EnumValue b) { return (EnumValue &)(((unsigned int &)a) ^= ((unsigned int)b)); } \
	inline EnumType operator | (EnumType a, EnumValue b) { return EnumType(((unsigned int)a) | ((unsigned int)b)); } \
	inline EnumType &operator |= (EnumType &a, EnumValue b) { return (EnumType &)(((unsigned int &)a) |= ((unsigned int)b)); } \
	inline EnumType operator & (EnumType a, EnumValue b) { return EnumType(((unsigned int)a) & ((unsigned int)b)); } \
	inline EnumType &operator &= (EnumType &a, EnumValue b) { return (EnumType &)(((unsigned int &)a) &= ((unsigned int)b)); } \
	inline EnumType operator ^ (EnumType a, EnumValue b) { return EnumType(((unsigned int)a) ^ ((unsigned int)b)); } \
	inline EnumType &operator ^= (EnumType &a, EnumValue b) { return (EnumType &)(((unsigned int &)a) ^= ((unsigned int)b)); }
