#pragma once

namespace mce {
	namespace Internal {
		// If you don't understand template instantiation across multiple compilers, don't remove this.
		template<typename T>
		struct AlwaysFalseStruct {
			static const bool value = false;
		};

	}
}
