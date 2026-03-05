#pragma once

namespace mce {
	template<typename EnumType, EnumType ActualType, typename UnderlyingType = typename std::underlying_type<EnumType>::type>
	class IsA {
	public:

		static EnumType getActualType() {
			return ActualType;
		}

		const static EnumType ImplementedType = ActualType;
	};

}
